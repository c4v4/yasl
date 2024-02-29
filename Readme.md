# Yet Another Sorting Library

I fooled around with different sorting algorithms. 
I wanted to see if I could do it faster than `std::sort` when I could make some extra assumptions (spoiler: yes, and it's remarkably easy). 
Here are the assumptions made:

- I can use extra O(N) space if it helps make things faster.
- The things I'm sorting can be represented as simple numerical types (like integers or floating points)
