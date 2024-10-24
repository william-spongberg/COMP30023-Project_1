# Project 1: Process and Memory Management

*Created for COMP30023 Computer Systems in Semester 1 of 2024 for the University of Melbourne.*

*Authored by William Spongberg and Lewis Zhang*

---

C program to simulate process and memory management.

Find assignment details [here](/COMP30023_2024_Project1.pdf).

## Testing

### Task 1

./allocate -f cases/task1/spec.txt -q 1 -m infinite | diff - cases/task1/spec-q1.out
./allocate -f cases/task1/two-processes.txt -q 1 -m infinite | diff - cases/task1/two-processes-q1.out
./allocate -f cases/task1/two-processes.txt -q 3 -m infinite | diff - cases/task1/two-processes-q3.out

### Task 2

./allocate -f cases/task2/fill.txt -q 3 -m first-fit | diff - cases/task2/fill-q3.out
./allocate -f cases/task2/non-fit.txt -q 1 -m first-fit | diff - cases/task2/non-fit-q1.out
./allocate -f cases/task2/retake-left.txt -q 3 -m first-fit | diff - cases/task2/retake-left-q3.out
./allocate -f cases/task2/consecutive-running.txt -q 3 -m first-fit | diff - cases/task2/consecutive-running-q3.out

### Task 3

./allocate -f cases/task3/simple-alloc.txt -q 3 -m paged | diff - cases/task3/simple-alloc-q3.out
./allocate -f cases/task3/simple-evict.txt -q 1 -m paged | diff - cases/task3/simple-evict-q1.out
./allocate -f cases/task3/internal-frag.txt -q 1 -m paged | diff - cases/task3/internal-frag-q1.out

### Task 4

./allocate -f cases/task4/no-evict.txt -q 3 -m virtual | diff - cases/task4/no-evict-q3.out
./allocate -f cases/task4/virtual-evict.txt -q 1 -m virtual | diff - cases/task4/virtual-evict-q1.out
./allocate -f cases/task4/virtual-evict-alt.txt -q 1 -m virtual | diff - cases/task4/virtual-evict-alt-q1.out
./allocate -f cases/task4/to-evict.txt -q 3 -m virtual | diff - cases/task4/to-evict-q3.out

### Mixed Tasks

./allocate -f cases/task1/spec.txt -q 1 -m infinite | diff - cases/task1/spec-q1.out
./allocate -f cases/task2/non-fit.txt -q 3 -m first-fit | diff - cases/task2/non-fit-q3.out
./allocate -f cases/task3/simple-alloc.txt -q 3 -m paged | diff - cases/task3/simple-alloc-q3.out
./allocate -f cases/task4/to-evict.txt -q 3 -m virtual | diff - cases/task4/to-evict-q3.out

## Debugging

valgrind --leak-check=full --show-leak-kinds=all ./[test case]
gdb --args ./[test case]
