import copy
import sys
import time

def is_time_over():
    if time.time() - start_time < 200:
        return False
    return True       

def get_local_std_var():
    return "aa#"

def read_input(filename):

    sentences = open(filename).read().split('\n')
    query = (sentences[0]).replace("~~", "")
    input_base = sentences[2:2 + int(sentences[1])]   
    kb = []
    kb.append(("~" + query).replace("~~", ""))
    for input in input_base:
        input = input.replace("~~", "")
        if input not in kb:
            kb.append(input)
    return kb

def write_output(found_contradiction):
    output = open('output.txt', 'w')
    output.write(found_contradiction)
    output.close()

def cnf_conversion(kb):
    cnf_kb = []
    for rule in kb:

        parsed_rule = extract_implication(rule)
        cnf_rules = perform_distributive_law(parsed_rule[0], parsed_rule[1])
        for cnf_rule in cnf_rules:
            cnf_rule = cnf_rule.replace(' | ', '%|%')
            cnf_rule = cnf_rule.replace(' ', '')
            cnf_rule = cnf_rule.replace('%|%', ' | ')
            cnf_rule, _ = make_variables_standardize(cnf_rule, get_local_std_var())

            cnf_kb.append(cnf_rule)
    return cnf_kb

def extract_implication(rule):
    if '=>' in rule:
        rule = rule.split('=>', 1)
        return [send_negation_inwards(rule[0].strip()), rule[1].strip()]
    return [rule, None]

def send_negation_inwards(sentence):
    if '~~' in sentence:
        new_sentence = sentence.replace('~~', '')
    elif '|' in sentence:
        sentence = sentence.split('|', 1)
        new_sentence = send_negation_inwards(sentence[0].strip()) + ' & ' + send_negation_inwards(sentence[1].strip())
    elif '&' in sentence:
        sentence = sentence.split('&', 1)
        new_sentence = send_negation_inwards(sentence[0].strip()) + ' | ' + send_negation_inwards(sentence[1].strip())
    else:
        new_sentence = "~" + sentence
    return new_sentence

def perform_distributive_law(rule, right):
    final_rule, rule = [], rule.replace('~~', '')
    if (right):
        final_rule = [rule.strip() + ' | ' + right for rule in rule.split('&')]
    else:
        new_rules = []
        for r in rule.split('|'):
            if '&' in r:
                new_rules.append([i.strip() for i in r.strip().split('&')])
            else:
                new_rules.append([r.strip()])

        def make_combinations(new_rules, temp):
            if (len(temp) == len(new_rules)):
                final_rule.append(copy.deepcopy(temp))
                return
            for i in new_rules[len(temp)]:
                temp.append(i)
                make_combinations(new_rules, temp)
                temp.pop()

        make_combinations(new_rules, [])
        final_rule = [' | '.join(rule) for rule in final_rule]
    return final_rule

def create_predicates_lookup(cnf_kb, cnf_lookup, offset = 0):
    global max_length
    for idx, rule in enumerate(cnf_kb):
        
        predicates = rule.split(' | ')
        length = len(predicates)
        max_length = max(max_length, length)    
        
        if length not in cnf_lookup:
            cnf_lookup[length] = {} 
        
        for p_idx, pred in enumerate(predicates):

            if ('(' in pred):
                pred_names = pred.strip().split('(')[0]
                pred_vars = pred.strip().split('(')[1].split(')')[0].split(',')
            else:
                pred_names = pred.strip()
                pred_vars = []

            for v_idx, var in enumerate(pred_vars):
                pred_vars[v_idx] = var.strip()
 
            if pred_names in cnf_lookup[length]:
                cnf_lookup[length][pred_names].append({'vars': pred_vars, 'rule': idx + offset, 
                                                       'index': p_idx, 'length': length})
            else:
                cnf_lookup[length][pred_names] = [{'vars': pred_vars, 'rule': idx + offset, 
                                                   'index': p_idx, 'length': length}]

    return cnf_lookup

def filter_repetitions(rules):
    sorted_rules = sorted(rules.split(' | '))
    unique_rules_set = set(sorted_rules)
    unique_rules = []
    for rule in sorted_rules:
        if rule in unique_rules_set:
            unique_rules.append(rule)
            unique_rules_set.remove(rule)
    return " | ".join(unique_rules)

def is_valid_rule(rule):
    predicates = rule.split(' | ')
    for predicate in predicates:
        if ("~" + predicate).replace("~~", "") in predicates:
            return False
    return True

def replace_variable(unification_string, old_var, new_var):

    unification_string = unification_string.replace("(" + old_var + ",", "(" + new_var + ",")
    unification_string = unification_string.replace("," + old_var + ",", "," + new_var + ",")
    unification_string = unification_string.replace("," + old_var + ")", "," + new_var + ")")
    unification_string = unification_string.replace("(" + old_var + ")", "(" + new_var + ")")
    return unification_string

def standardize_before_unification(var):
    if (ord(var[1]) == ord('Z')):
        var = chr(ord(var[0])+1) + 'A#'
    else:
        var = var[0] + chr(ord(var[1])+1) + '#'
    return var

def standardize_local(std_var):
    if (ord(std_var[1]) == ord('z')):
        std_var = chr(ord(std_var[0])+1) + 'a%'
    else:
        std_var = std_var[0] + chr(ord(std_var[1])+1) + '%'
    return std_var

def make_variables_standardize(rule, temp):
    
    local_var_map, new_rule = {}, rule
    for start in range(len(rule)):
        if (rule[start] == '('):
            for end in range(start + 1, len(rule), 1):
                if (rule[end] == ')'):
                    vars = rule[start + 1:end].split(',')
                    for j in range(len(vars)):
                        if (vars[j][0].islower() and vars[j] not in local_var_map and '#' not in vars[j]):
                            temp = standardize_local(temp).replace('%', '#')
                            temp = temp.replace('#', '%')
                            local_var_map[vars[j]] = temp
                    break
    new_rule = rule
    for var in reversed(sorted(local_var_map.keys())):
        new_rule = replace_variable(new_rule, var, local_var_map[var])
    new_rule = new_rule.replace('%', '#')
    return new_rule, temp.replace('%', '#')

def can_be_unified(var_arr_1, var_arr_2):
    for i in range(len(var_arr_1)):
        if var_arr_2[i] != var_arr_1[i] and var_arr_2[i][0].isupper() and var_arr_1[i][0].isupper():
            return False
    return True

def unify_two_rules(left_rule, right_rule, left_idx, right_idx, left_vars, right_vars):

    if not can_be_unified(left_vars, right_vars):
        return "", False
    else:
        local_std_var = "aA#"
        for i, var in enumerate(left_vars):
            if var[0].isupper():
                right_rule = replace_variable(right_rule, right_vars[i], var)
            elif right_vars[i][0].isupper():
                left_rule = replace_variable(left_rule, var, right_vars[i])
            else:
                local_std_var = standardize_before_unification(local_std_var)
                left_rule = replace_variable(left_rule, var, local_std_var)
                right_rule = replace_variable(right_rule, right_vars[i], local_std_var)

    predicates_left_rule = left_rule.split(' | ')
    predicates_right_rule = right_rule.split(' | ')

    if (predicates_left_rule[left_idx][1:] != predicates_right_rule[right_idx] 
        and predicates_left_rule[left_idx] != predicates_right_rule[right_idx][1:]):
        return "", False

    del predicates_left_rule[left_idx]
    del predicates_right_rule[right_idx]

    left_rule = ' | '.join(predicates_left_rule)
    right_rule = ' | '.join(predicates_right_rule)

    if (right_rule == "" or left_rule == ""):
        unified_rule = left_rule + right_rule
    else:
        left_rule, next_term = make_variables_standardize(left_rule, 'aa#')
        right_rule, _ = make_variables_standardize(right_rule, next_term)
        unified_rule = left_rule + " | " + right_rule

    unified_rule = filter_repetitions(unified_rule)
    unified_rule, _ = make_variables_standardize(unified_rule, get_local_std_var())
    if not is_valid_rule(unified_rule):
        return "", False
    
    return unified_rule, True

def update_kb_lookup(cnf_kb, inferred_cnf_base, cnf_lookup):
    total_sen_in_kb = len(cnf_kb)
    new_cnf_base = []
    for rule in inferred_cnf_base:
        if rule not in cnf_kb:
            new_cnf_base.append(rule)
            cnf_kb.append(rule)
    cnf_lookup = create_predicates_lookup(new_cnf_base, cnf_lookup, offset = total_sen_in_kb)
    return cnf_kb, cnf_lookup

def unit_to_single_rule_unify(cnf_kb, cnf_lookup, prev_found_rule):
    
    length = len(prev_found_rule.split(' | '))
    new_lookup = create_predicates_lookup([prev_found_rule], {}, offset = len(cnf_kb)-1)        
    j_length_rules = new_lookup[length]
    unit_length_rules = cnf_lookup[1]
    contra_detected = False
    
    for predicate in j_length_rules:
    
        not_predicate = ('~' + predicate).replace('~~', '')
        if not_predicate not in unit_length_rules:
            continue
        
        unit_rules = unit_length_rules[not_predicate]
        for i in range(len(j_length_rules[predicate]) -1, -1, -1):

            j_rule = j_length_rules[predicate][i]    
            j_rule_cnf = cnf_kb[j_rule['rule']]
            j_rule_position = j_rule['index']
            j_rule_vars = j_rule['vars']

            for unit_rule in unit_rules:
                
                unit_rule_cnf = cnf_kb[unit_rule['rule']]
                unit_rule_position = unit_rule['index']
                unit_rule_vars = unit_rule['vars']
                unified_rule, is_valid = unify_two_rules(j_rule_cnf, unit_rule_cnf, j_rule_position, 
                                                         unit_rule_position, j_rule_vars, unit_rule_vars)
                if  is_valid: 
                    if unified_rule == "":
                        contra_detected = True
                        return 0, contra_detected, cnf_kb, cnf_lookup
                    if unified_rule in cnf_kb:
                        continue
                    
                    cnf_kb, cnf_lookup = update_kb_lookup(cnf_kb, [unified_rule], cnf_lookup)
                    rec_length, contra_detected, cnf_kb, cnf_lookup = unit_to_single_rule_unify(cnf_kb, cnf_lookup, unified_rule)
                    length = min(length, rec_length)
                    if contra_detected:
                        return 0, contra_detected, cnf_kb, cnf_lookup
                if is_time_over():
                    return 0, contra_detected, cnf_kb, cnf_lookup
                                    
    return length, contra_detected, cnf_kb, cnf_lookup 

def multi_to_multi_rule_unify(cnf_kb, cnf_lookup, h_value, j_value):

    contra_detected, go_to_unit_step = False, False
    minimum_length = max(j_value, h_value)

    if h_value not in cnf_lookup or j_value not in cnf_lookup:
       return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup
    
    j_length_rules = copy.deepcopy(cnf_lookup[j_value])
    h_length_rules = copy.deepcopy(cnf_lookup[h_value])
    for predicate in j_length_rules.keys():

        not_predicate = ('~' + predicate).replace('~~', '')
        if not_predicate not in h_length_rules:
            continue

        h_length_rules_negate = h_length_rules[not_predicate]
        for i in range(len(j_length_rules[predicate]) - 1, -1, -1):

            j_rule = j_length_rules[predicate][i]    
            j_rule_cnf = cnf_kb[j_rule['rule']]
            j_rule_position = j_rule['index']
            j_rule_vars = j_rule['vars']

            for h_clause_obj in h_length_rules_negate:
            
                h_rule_cnf = cnf_kb[h_clause_obj['rule']]
                h_rule_position = h_clause_obj['index']
                h_rule_vars = h_clause_obj['vars']
            
                unified_rule, is_valid = unify_two_rules(j_rule_cnf, h_rule_cnf, j_rule_position, 
                                                         h_rule_position, j_rule_vars, h_rule_vars)
                
                if is_valid:
                    if unified_rule == "":
                        contra_detected = True
                        return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup
                    if unified_rule in cnf_kb:
                        continue
                    
                    cnf_kb, cnf_lookup = update_kb_lookup(cnf_kb, [unified_rule], cnf_lookup)
                    length, contra_detected, cnf_kb, cnf_lookup = unit_to_single_rule_unify(cnf_kb, cnf_lookup, unified_rule)
                    if contra_detected:
                        return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup
                    
                    minimum_length = min(minimum_length, length)
                    if minimum_length == 1:
                        go_to_unit_step = True
                        return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup
                if is_time_over():
                    return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup
                                           
    return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup

def factorize_rule(rule, var_arr_1, var_arr_2, index_1, index_2):

    if not can_be_unified(var_arr_1, var_arr_2):
        return "", False
    else:
        local_std_var = "aA#"
        for i, var in enumerate(var_arr_1):
            if var[0].isupper():
                rule = replace_variable(rule, var_arr_2[i], var)
            elif var_arr_2[i][0].isupper():
                rule = replace_variable(rule, var, var_arr_2[i])
            else:
                local_std_var = standardize_before_unification(local_std_var)
                rule = replace_variable(rule, var, local_std_var)
                rule = replace_variable(rule, var_arr_2[i], local_std_var)

    predicates = rule.split(' | ')
    if (predicates[index_1] != predicates[index_2]):
        return "", False
    del predicates[index_1]

    new_rule = ' | '.join(predicates)
    new_rule, _ = make_variables_standardize(new_rule, 'aa#')
    new_rule = filter_repetitions(new_rule)
    new_rule, _ = make_variables_standardize(new_rule, get_local_std_var())

    if not is_valid_rule(new_rule):
        return "", False
    return new_rule, True

def unit_to_multi_rule_unify(cnf_kb, cnf_lookup, j_value):

    j_length_rules = cnf_lookup[j_value]
    unit_length_rules = cnf_lookup[1]
    contra_detected, new_rules_found = False, False
    
    for predicate in j_length_rules:

        not_predicate = ('~' + predicate).replace('~~', '')
        if (not_predicate not in unit_length_rules):
            continue

        for i in range(len(j_length_rules[predicate]) - 1, -1, -1):

            j_rule = j_length_rules[predicate][i]    
            j_rule_cnf = cnf_kb[j_rule['rule']]
            j_rule_position = j_rule['index']
            j_rule_vars = j_rule['vars']

            for unit_rule in unit_length_rules[not_predicate]:
                
                unit_rule_cnf = cnf_kb[unit_rule['rule']]
                unit_rule_position = unit_rule['index']
                unit_rule_vars = unit_rule['vars']

                unified_rule, is_valid = unify_two_rules(j_rule_cnf, unit_rule_cnf, j_rule_position, 
                                                         unit_rule_position, j_rule_vars, unit_rule_vars)
                
                if is_valid:
                    if unified_rule == "":
                        contra_detected = True
                        return 0, new_rules_found, contra_detected, unified_rule, cnf_kb, cnf_lookup
                    if unified_rule in cnf_kb:
                        continue

                    cnf_kb, cnf_lookup = update_kb_lookup(cnf_kb, [unified_rule], cnf_lookup)
                    new_rules_found = True
                    return len(unified_rule.split(' | ')), new_rules_found, contra_detected, unified_rule, cnf_kb, cnf_lookup
                
                if is_time_over():
                    return 0, new_rules_found, contra_detected, unified_rule, cnf_kb, cnf_lookup
                
    return j_value + 1, new_rules_found, contra_detected, "", cnf_kb, cnf_lookup
        
def apply_unit_step(cnf_kb, cnf_lookup):

    j_value, contra_detected  = 1, False
    new_rules_found_prev, prev_found_rule = False, ""
    while j_value <= max_length:

        if j_value not in cnf_lookup:
            j_value = j_value + 1
            continue

        if new_rules_found_prev:    
            new_j_value, contra_detected, cnf_kb, cnf_lookup = unit_to_single_rule_unify(cnf_kb, cnf_lookup, prev_found_rule)
            new_rules_found_prev, prev_found_rule = False, ""
            if contra_detected:
                return contra_detected, cnf_kb, cnf_lookup
            if new_j_value == 1:
                j_value = 1
            else:
                j_value += 1
        else:
            new_j_value, is_new_value_added, contra_detected, unified_clause, cnf_kb, cnf_lookup = unit_to_multi_rule_unify(cnf_kb, cnf_lookup, j_value)
            if contra_detected:
                return contra_detected, cnf_kb, cnf_lookup
            if is_new_value_added:
                prev_found_rule = unified_clause
                new_rules_found_prev = True
                j_value = new_j_value
            else: 
                new_rules_found_prev = False
                j_value += 1
        
        if is_time_over():
            return contra_detected, cnf_kb, cnf_lookup

    return contra_detected, cnf_kb, cnf_lookup

def factorization(cnf_kb, cnf_lookup, j_value):

    perform_unit_step, contra_detected = False, False
    if j_value not in cnf_lookup:
        return perform_unit_step, contra_detected, cnf_kb, cnf_lookup
    
    j_length_rules = cnf_lookup[j_value]
    for predicate in j_length_rules:
        
        total_predicates = len(j_length_rules[predicate])
        for i in range(total_predicates - 1, -1, -1):
            
            j_rule_1 = j_length_rules[predicate][i]
            j_rule_1_cnf = cnf_kb[j_rule_1['rule']]
            j_rule_1_position = j_rule_1['index']
            j_rule_1_vars = j_rule_1['vars']

            for j in range(total_predicates - 1, -1, -1):
                if i != j:
                    j_rule_2 = j_length_rules[predicate][j]
                    j_rule_2_position = j_rule_2['index']
                    j_rule_2_vars = j_rule_2['vars']

                    if j_rule_2['rule'] == j_rule_1['rule']:
                        factorized_rule, is_valid = factorize_rule(j_rule_1_cnf, j_rule_1_vars, j_rule_2_vars, 
                                                                   j_rule_1_position, j_rule_2_position)                  
                        if is_valid:
                            if factorized_rule in cnf_kb:
                                continue

                            cnf_kb, cnf_lookup = update_kb_lookup(cnf_kb, [factorized_rule], cnf_lookup)
                            final_length, contra_detected, cnf_kb, cnf_lookup = unit_to_single_rule_unify(cnf_kb, cnf_lookup, factorized_rule)

                            if contra_detected:
                                return perform_unit_step, contra_detected, cnf_kb, cnf_lookup
                            if final_length == 1: 
                                perform_unit_step = True
                                return perform_unit_step, contra_detected, cnf_kb, cnf_lookup
            
    return perform_unit_step, contra_detected, cnf_kb, cnf_lookup

def apply_non_unit_step(cnf_kb, cnf_lookup):
    
    j_value, h_value = 2, 2
    while h_value <= max_length:  

        go_to_unit_step, contra_detected, cnf_kb, cnf_lookup = factorization(cnf_kb, cnf_lookup, j_value)
        if contra_detected or go_to_unit_step:
            return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup 
        if j_value == 2:
            j_value += 1
        else:
            h_value, j_value = 2, j_value - 1
            while h_value <= j_value:

                go_to_unit_step, contra_detected, cnf_knowledge_base, cnf_lookup = multi_to_multi_rule_unify(cnf_kb, cnf_lookup, h_value, j_value)
                if contra_detected or go_to_unit_step:
                    return go_to_unit_step, contra_detected, cnf_knowledge_base, cnf_lookup     
                
                h_value += 1
                j_value -= 1

                if is_time_over():
                    return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup

            j_value = h_value + j_value
        
        if is_time_over():
            return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup
    
    return go_to_unit_step, contra_detected, cnf_kb, cnf_lookup

def get_inference(cnf_kb, cnf_lookup):
    
    new_rules_found = True
    while(new_rules_found):

        new_rules_found = False 
        contra_detected, cnf_kb, cnf_lookup = apply_unit_step(cnf_kb, cnf_lookup)
        if contra_detected:
            return True 
        if is_time_over():
            return False
                  
        perform_unit_step_again, contra_detected, cnf_kb, cnf_lookup = apply_non_unit_step(cnf_kb, cnf_lookup)
        if contra_detected:
            return True 
        if is_time_over():
            return False  
        if perform_unit_step_again:
            new_rules_found = True    
    return False

max_length = 0
start_time = time.time()
if __name__ == "__main__":
    args = sys.argv
    try: 
        write_output("FALSE")
        kb = read_input(args[1])
        cnf_kb = cnf_conversion(kb)
        cnf_lookup = create_predicates_lookup(cnf_kb, {})   
        answer = str(get_inference(cnf_kb, cnf_lookup)).upper()
        write_output(answer)
    except Exception as e:
        write_output("FALSE")