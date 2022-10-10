This is my solution for the test round problem of 2018 Google Hashcode challenge.

# The problem
The problem statement is placed in `pizza.pdf` file. All data provided by Google are in
`input_data` directory.

I have seen the excellent solution written by tompil https://github.com/tompil/Google-Hashcode-Pizza-Problem-Solution
which runs over the cells of the pizza and cut slices for each cell when it possible.
After the first process the final-slices vector created. in the second process it try to
extend all the slices where it possible.

I try to solve it by ranking the slices - (groups of pixels) according to the number of other groups it share.

**The algorithm is:**
**1.** Go over the size of slice restrictions and build all the possible shapes for slice.
for example, if min_each is 2 and max cells in group is 6, then the num of pixels in slices is between 4-6.
because the slices are rectangular we get 9 shapes possible

**2.** For each pixel on the grid try to build group that start with that pixel for every shape. if such group is legal,
append it to legal gruops.

**3.** Rank each group according to the number of other groups it share cells with.

**4.** Add negative rank for slim groups. slim group is more likely to be extendable after we choose the final groups

**5.** Choose the groups with the least rank one by one. when choose group:
    1. delete it from the set of legel groups.
    2. delete every group that hold any of the cells inside the group you choose.
    3. for every group that hed shared cells with any of the deleted groups, correct it score according to the number of 
    the deleted groups it is not share anymore.

**6.** Try to extend groups.

I implement stage 3 by multi - threading with 4 threads working simultaneously. whereas stage 5 that take the majority 
of the time, is more dificult to imlement in multi threading, and I doubt if it can shortern the running time significantly.

# Results
Full solution is in `output_data` directory.

The run on the medium data take some minute, while on the big data it took up to half an hour. 

Here are the points:

| Case      | Points | Out of  |
|-----------|--------|---------|
| example   | 15     | 15      |
| small     | 39     | 42      |
| medium    | 48904  | 50000   |
| big       | 901750 | 1000000 |
| **TOTAL** | 950708 |         |




