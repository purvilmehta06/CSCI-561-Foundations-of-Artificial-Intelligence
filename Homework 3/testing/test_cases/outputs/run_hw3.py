#!/usr/bin/env python3
from subprocess import DEVNULL, run, CalledProcessError, DEVNULL, TimeoutExpired
from tempfile import TemporaryDirectory
import shutil
import argparse
import os
from typing import Dict, Tuple, Union, List
from glob import glob
from os.path import abspath
import time

N_CASES = 50


def printv(verbose: bool, *args, **kwargs):
    if verbose:
        print(*args, **kwargs)


def run_hw3(
    data_dir: str,
    cmd_to_run: str,
    env: Dict[str, str],
    n_cases: int,
    verbose: bool,
    timeout: Union[int, None],
    write_outputs: bool,
    capture_output: bool = True,
) -> List[bool]:
    failed_cases = []
    success_cases = []
    for i in range(1, 1 + n_cases):
        fail_msg = "FAIL"
        # directory = f"{data_dir}/test_case_{i}/"
        # os.makedirs(directory, exist_ok=True)
        # if True:
        with TemporaryDirectory() as directory:
            shutil.copy2(f"{data_dir}/input{i}.txt", f"{directory}/input.txt")
            with open(f"{data_dir}/output{i}.txt") as f:
                expected_output = f.readlines()
            start_time = time.time()
            out_pipe = None
            if capture_output:
                out_pipe = DEVNULL
            try:
                if " " in cmd_to_run:
                    run(
                        cmd_to_run,
                        shell=True,
                        cwd=directory,
                        check=True,
                        stdout=out_pipe,
                        stderr=out_pipe,
                        timeout=timeout,
                        env=env,
                    )
                else:
                    run(
                        [abspath(cmd_to_run)],
                        cwd=directory,
                        check=True,
                        stdout=out_pipe,
                        stderr=out_pipe,
                        timeout=timeout,
                        env=env,
                    )
            except CalledProcessError:
                printv(verbose, "Student process crashed")
                fail_msg = "CRASH"
                # Don't need to do anything to penalize the student process
                # failing
            except TimeoutExpired:
                printv(verbose, "Student process timed out")
                fail_msg = "TIMEOUT"
                # Don't need to do anything to penalize the student process
                # failing
            printv(
                verbose,
                f"input{i}.txt took {int(0.5 + time.time() - start_time)} seconds",
            )

            try:
                with open(f"{directory}/output.txt") as f:
                    student_output = f.readlines()
                if write_outputs:
                    # Write outputs if reference solution
                    shutil.copy2(
                        f"{directory}/output.txt", f"{directory}/output{i}.txt"
                    )
            except FileNotFoundError:
                printv(verbose, "output.txt not found")
                failed_cases.append(i)
                print(f"input{i}.txt: {fail_msg}")
                continue

        if student_output[0].strip() == expected_output[0].strip():
            success_cases.append(i)
            print(f"input{i}.txt: PASS")
        else:
            failed_cases.append(i)
            print(f"input{i}.txt: {fail_msg}")
    total_cases = len(success_cases) + len(failed_cases)
    print(f"PASS: {len(success_cases)}/{total_cases}")
    print(f"FAIL: {len(failed_cases)}/{total_cases}")
    return failed_cases


def prepare_cmd(student_dir: str) -> Tuple[str, Dict[str, str]]:
    student_dir = abspath(student_dir)
    files = os.listdir(student_dir)
    env = os.environ.copy()
    if "homework.cpp" in files:
        try:
            run(
                ["g++", "-std=c++11", "homework.cpp", "-o", "exe"],
                check=True,
                cwd=student_dir,
            )
        except CalledProcessError:
            pass
        else:
            return (f"{student_dir}/exe", env)
    if "homework.java" in files:
        try:
            run(
                [
                    "javac",
                ]
                + glob(f"{student_dir}/*.java"),
                check=True,
                cwd=student_dir,
            )
        except CalledProcessError:
            pass
        else:
            env["CLASSPATH"] = student_dir
            return ("java homework", env)
    if "homework.py" in files:
        env["PYTHONPATH"] = student_dir
        return (f"python {student_dir}/homework.py", env)
    raise FileNotFoundError("No homework file found")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data-dir", type=str, default=".")
    parser.add_argument("--command-to-run", type=str, default=None)
    parser.add_argument("--student-dir", type=str, default=None)
    parser.add_argument("--n-cases", type=int, default=N_CASES)
    parser.add_argument("--verbose", action="store_true")
    parser.add_argument("--timeout", type=float, default=20 * 60.0)
    parser.add_argument("--write-outputs", action="store_true")
    parser.add_argument("--nocapture", action="store_true")
    args = parser.parse_args()
    if args.student_dir is not None:
        assert args.command_to_run is None
        command, env = prepare_cmd(args.student_dir)
    else:
        assert args.student_dir is None
        command = args.command_to_run
        env = os.environ.copy()
    run_hw3(
        args.data_dir,
        command,
        env,
        args.n_cases,
        args.verbose,
        args.timeout,
        args.write_outputs,
        capture_output=not args.nocapture,
    )
