This is my solution for the test round problem of 2018 Google Hashcode challenge.

I have seen the excellent solution written by tompil https://github.com/tompil/Google-Hashcode-Pizza-Problem-Solution
which runs over the cells of the pizza and cut slices for each cell when it posible.
After the first process the final-slices vector created. in the second process it try to
extend all the slices where it posible.

I am tring to solve it by ranking the slices - (groups of pixels) according to the number of other groups it share.

**The algorithm is:**
**1.** Go over the size of slice restrictions and biuld all the posible shapes for slice.
for example, if min_each is 2 and max is 6, then the num of pixelsin slices is between 4-6.
because the slices are rectangular we get 9 shapes possible

**2.** For each pixel try to build group that start with that pixel for every shape. if such group is legal,
append it to legal gruops.

**3.** Rank all the groups according to the num of other groups it share.

**4.** Add negative rank for slim groups. slim group is more likely to be extendable at the end of.

**5.** Chose the groups with the least rank.

**6.** Try to extend groups.


# The problem
The problem statement is placed in `pizza.pdf` file. All data provided by Google are in
`input_data` directory.
# Results
Full solution is in `output_data` directory.

The run on the medium data take some minute, while on the big data it took up to half an hour

Here are the points:

| Case      | Points |
|-----------|--------|
| example   | 15     |
| small     | 39     |
| medium    | 48904  |
| big       | 901750 |
| **TOTAL** | 950708 |




