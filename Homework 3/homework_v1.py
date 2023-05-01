import copy
import sys

def get_local_std_var():
    return "aa#"

def read_input(filename):
    lines = open(filename).read().split('\n')
    query = (lines[0]).replace("~~", "")
    negation_query = ( "~" + query).replace("~~", "")
    knowledge_base = lines[2:2 + int(lines[1])]    
    knowledge_base.append(negation_query)
    knowledge_base = [cnf.replace(" ", "") for cnf in knowledge_base]
    return negation_query, knowledge_base

def write_output(answer):
    output = open('output.txt', 'w')
    output.write(answer)
    output.close()

def cnf_conversion(knowledge_base):
    cnf_kb = []
    for rule in knowledge_base:
        updated_rule = parse_implications(rule)
        cnf_rules = apply_distributive_law(updated_rule[0], updated_rule[1])
        for cnf_rule in cnf_rules:
            cnf_rule = cnf_rule.replace(' | ', '%|%')
            cnf_rule = cnf_rule.replace(' ', '')
            cnf_rule = cnf_rule.replace('%|%', ' | ')
            temp = get_local_std_var()
            cnf_rule, _ = make_variables_standardize(cnf_rule, temp, {})
            cnf_kb.append(cnf_rule)
    return cnf_kb

def parse_implications(rule):
    if '=>' in rule:
        rule = rule.split('=>', 1)
        return [distribute_negation(rule[0].strip()), rule[1].strip()]
    return [rule, None]

def distribute_negation(rule):
    if '~~' in rule:
        new_rule = rule.replace('~~', '')
    elif '|' in rule:
        rule = rule.split('|', 1)
        new_rule = distribute_negation(
            rule[0].strip()) + ' & ' + distribute_negation(rule[1].strip())
    elif '&' in rule:
        rule = rule.split('&', 1)
        new_rule = distribute_negation(
            rule[0].strip()) + ' | ' + distribute_negation(rule[1].strip())
    else:
        new_rule = "~" + rule
    return new_rule

def apply_distributive_law(rule, right):
    final_rule = []
    rule = rule.replace('~~', '')

    if (not right):
        temp_rules = []
        for r in rule.split('|'):
            if '&' in r:
                temp = r.strip().split('&')
                temp_rules.append([i.strip() for i in temp])
            else:
                temp_rules.append([r.strip()])

        def make_combinations(temp_rules, temp):
            if (len(temp) == len(temp_rules)):
                final_rule.append(copy.deepcopy(temp))
                return
            for i in temp_rules[len(temp)]:
                temp.append(i)
                make_combinations(temp_rules, temp)
                temp.pop()
        make_combinations(temp_rules, [])
        final_rule = [' | '.join(rule) for rule in final_rule]
    else:
        final_rule = [rule.strip() + ' | ' + right for rule in rule.split('&')]

    return final_rule

def print_kb(query, kb):
    print("Query: {}\n".format(query))
    print("Knowledge Base (CNF):")
    for i, rule in enumerate(kb):
        print("{}. {}". format(i + 1, rule))

def print_dict(dic, name):
    print("\n" + name)
    for key, value in dic.items():
        print('{}  :  {}'.format(key, value))

def check_for_atomic_sentence(rule):
    for predicate in rule.split(' | '):
        predicate = predicate.strip()        
        if '(' in predicate:
            vars = predicate.split('(')[1].split(')')[0].split(',')
            for var in vars:
                if not var.strip()[0].isupper():
                    return False
    return True

def create_predicates_lookup(cnf_kb, atomic_predicate_lookup, variable_predicate_lookup, offset = 0):
    
    for idx, rule in enumerate(cnf_kb):
        predicates = rule.split(' | ')
        is_atomic_sentence = check_for_atomic_sentence(rule)
        for p_index, predicate in enumerate(predicates):
            if ('(' in predicate):
                pred_name = predicate.strip().split('(')[0]
                pred_vars = predicate.strip().split('(')[1].split(')')[0].split(',')
            else:
                pred_name = predicate.strip()
                pred_vars = []

            for v_index, variable in enumerate(pred_vars):
                pred_vars[v_index] = variable.strip()

            if is_atomic_sentence:
                if pred_name not in atomic_predicate_lookup:
                    atomic_predicate_lookup[pred_name] = [{'vars': pred_vars, 'rule': idx + offset, 'index': p_index , 'len': len(predicates) }]
                else:
                    atomic_predicate_lookup[pred_name].append({'vars': pred_vars, 'rule': idx + offset, 'index': p_index , 'len': len(predicates) })
            else:
                if pred_name not in variable_predicate_lookup:
                    variable_predicate_lookup[pred_name] = [{'vars': pred_vars, 'rule': idx + offset, 'index': p_index , 'len': len(predicates) }]
                else:
                    variable_predicate_lookup[pred_name].append({'vars': pred_vars, 'rule': idx + offset, 'index': p_index , 'len': len(predicates) })
    
    for key in atomic_predicate_lookup.keys():
        atomic_predicate_lookup[key] = sorted(atomic_predicate_lookup[key], key=lambda k: k['len'])
        
    for key in variable_predicate_lookup.keys():
        variable_predicate_lookup[key] = sorted(variable_predicate_lookup[key], key=lambda k: k['len'])
    
    return atomic_predicate_lookup, variable_predicate_lookup

def remove_same_terms(rules):
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

def replace_string(unified_string, query_var, var):
    unified_string = unified_string.replace("(" + query_var + ",", "(" + var + ",")
    unified_string = unified_string.replace("," + query_var + ",", "," + var + ",")
    unified_string = unified_string.replace("," + query_var + ")", "," + var + ")")
    unified_string = unified_string.replace("(" + query_var + ")", "(" + var + ")")
    return unified_string

def standardize_before_unify(var):
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

def make_variables_standardize(rule, temp, variables_in_unification_map):
    variable_map = {}
    new_rule = rule
    for ind in range(len(rule)):
        if (rule[ind] == '('):
            for i in range(ind+1, len(rule), 1):
                if (rule[i] == ')'):
                    variables = rule[ind+1:i].split(',')
                    for j in range(len(variables)):
                        if (variables[j][0].islower() and variables[j] not in variable_map):
                            temp = standardize_local(temp)
                            temp = temp.replace('%', '#')
                            while temp in variables_in_unification_map:
                                temp = standardize_local(temp)
                                temp = temp.replace('%', '#')
                            temp = temp.replace('#', '%')
                            variable_map[variables[j]] = temp
                    break
    new_rule = rule
    for var in reversed(sorted(variable_map.keys())):
        var = var.replace('%', '#')
        if var not in variables_in_unification_map:
            new_rule = replace_string(new_rule, var, variable_map[var])
    new_rule = new_rule.replace('%', '#')
    return new_rule, temp.replace('%', '#')

def is_unification_possible(var_arr_1, var_arr_2):
    for i in range(len(var_arr_1)):
        if var_arr_2[i] != var_arr_1[i] and var_arr_2[i][0].isupper() and var_arr_1[i][0].isupper():
            return False
    return True

def unify_two_rules(left_rule, right_rule, left_idx, right_idx, left_vars, right_vars, is_debug = False):

    if not is_unification_possible(left_vars, right_vars):
        return "", False
    else:
        local_std_var = "aA#"
        for i, var in enumerate(left_vars):
            if var[0].isupper():
                right_rule = replace_string(right_rule, right_vars[i], var)
            elif right_vars[i][0].isupper():
                left_rule = replace_string(left_rule, var, right_vars[i])
            else:
                local_std_var = standardize_before_unify(local_std_var)
                left_rule = replace_string(left_rule, var, local_std_var)
                right_rule = replace_string(right_rule, right_vars[i], local_std_var)

        if (is_debug): 
            print("Unification: ", left_rule, right_rule)

    predicates_left, predicates_right = left_rule.split(' | '), right_rule.split(' | ')
    if (predicates_left[left_idx][1:] != predicates_right[right_idx] 
        and predicates_left[left_idx] != predicates_right[right_idx][1:]):
        return "", False

    del predicates_left[left_idx]
    del predicates_right[right_idx]
    left_rule = ' | '.join(predicates_left)
    right_rule = ' | '.join(predicates_right)

    if (right_rule == ""):
        unified_rule = left_rule
    elif (left_rule == ""):
        unified_rule = right_rule
    else:
        left_rule, next_term = make_variables_standardize(left_rule, 'aa#', {})
        right_rule, _ = make_variables_standardize(right_rule, next_term, {})
        unified_rule = left_rule + " | " + right_rule
    unified_rule = remove_same_terms(unified_rule)
    unified_rule, _ = make_variables_standardize(unified_rule, get_local_std_var(), {})
    if not is_valid_rule(unified_rule):
        return "", False
    
    return unified_rule, True

def update_kb_lookup(cnf_kb, inferred_cnf_base, atomic_predicate_lookup, variable_predicate_lookup):
    total_sen_in_kb = len(cnf_kb)
    new_cnf_base = []
    for rule in inferred_cnf_base:
        if rule not in cnf_kb:
            new_cnf_base.append(rule)
            cnf_kb.append(rule)
    atomic_predicate_lookup, variable_predicate_lookup = create_predicates_lookup(new_cnf_base, atomic_predicate_lookup, 
                                                                                  variable_predicate_lookup, 
                                                                                  offset = total_sen_in_kb)
    return cnf_kb, atomic_predicate_lookup, variable_predicate_lookup

def forward_chaining(cnf_kb, lookup_1, lookup_2, inferred_cnf_base, new_rule_found, cutoff):

    for predicate in lookup_1.keys():
            not_predicate = ("~" + predicate).replace("~~", "")
            if not_predicate not in lookup_2:
                continue
            for p in lookup_1[predicate]:
                if p['len'] > cutoff:
                    continue
                else:
                    left_rule = cnf_kb[p['rule']] 
                    for np in lookup_2[not_predicate]:
                        right_rule = cnf_kb[np['rule']]
                        unified_rule, is_valid = unify_two_rules(left_rule, right_rule, p['index'], np['index'], p['vars'], np['vars']) 
                        if is_valid and unified_rule not in inferred_cnf_base and unified_rule not in cnf_kb: 
                            new_rule_found = True
                            inferred_cnf_base.append(unified_rule)
                            if (unified_rule == ""):
                                return [], True, True
                            return inferred_cnf_base, new_rule_found, False 
                    
    return inferred_cnf_base, new_rule_found, False            

def get_inference(cnf_kb, atomic_predicate_lookup, variable_predicate_lookup):
    
    cutoff = 2
    new_rule_found = True
    while(new_rule_found):
        new_rule_found, inferred_cnf_base = False, []       
        inferred_cnf_base, new_rule_found, contradiction_found = forward_chaining(cnf_kb, atomic_predicate_lookup, 
                                                                                  atomic_predicate_lookup, 
                                                                                  inferred_cnf_base, new_rule_found, cutoff)
        if (contradiction_found): 
            return True

        inferred_cnf_base, new_rule_found, contradiction_found = forward_chaining(cnf_kb, atomic_predicate_lookup, 
                                                                                  variable_predicate_lookup, 
                                                                                  inferred_cnf_base, new_rule_found, cutoff)
        if (contradiction_found): 
            return True
        
        inferred_cnf_base, new_rule_found, contradiction_found = forward_chaining(cnf_kb, variable_predicate_lookup, 
                                                                                  variable_predicate_lookup, 
                                                                                  inferred_cnf_base, new_rule_found, cutoff)
        if (contradiction_found): 
            return True  
        
        cnf_kb, atomic_predicate_lookup, variable_predicate_lookup = update_kb_lookup(cnf_kb, inferred_cnf_base, 
                                                                                      atomic_predicate_lookup, 
                                                                                      variable_predicate_lookup)

    return False

if __name__ == "__main__":

    args = sys.argv
    query, knowledge_base = read_input(args[1])
    cnf_kb = cnf_conversion(knowledge_base)
    atomic_predicate_lookup, variable_predicate_lookup = create_predicates_lookup(cnf_kb, {}, {})
    
    is_debug = False
    if (is_debug):
        print_kb(query, cnf_kb)
        print_dict(atomic_predicate_lookup, "Constant Lookup Table:")
        print_dict(variable_predicate_lookup, "Variable Lookup Table:")

    answer = str(get_inference(cnf_kb, atomic_predicate_lookup, variable_predicate_lookup)).upper()
    write_output(answer)