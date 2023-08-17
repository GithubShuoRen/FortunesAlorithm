# 1.**sites**

A Voronoi diagram is essentially a way to partition a plane into regions based on distances to a given set of **points**. These given points are called "sites".

![image-20230815214550526](imgs/image-20230815214550526.png)

## 1.1 site event

**Main Event Loop**

```cpp
while (!points.empty())
   if (!events.empty() && events.top()->x <= points.top().x)
      process_event(); // This handles circle events
   else
      process_point(); // This handles site events
```

The `process_point()` function handles a site event. When a site event is processed, a new parabola is introduced to the "beach line".

## 1.2 process_point()

```cpp
void process_point()
{
    // Get the next point from the queue.
    point p = points.top();
    points.pop();

    // Add a new arc to the parabolic front.
    front_insert(p);
}
```



## 1.3 front_insert

```cpp
void front_insert(point p)
{
   if (!root) {
      root = new arc(p);
      return;
   }

    
    /* The new parabola will intersect the beach line at one or two points, thus potentially dividing an existing arc into two parts. This is the reason why you might need to "split" an existing arc into two arcs and insert the new arc for site p between them. */
    
   // Find the current arc(s) at height p.y (if there are any).
   for (arc *i = root; i; i = i->next) {
      point z, zz;
      if (intersect(p,i,&z)) {
         // New parabola intersects arc i.  If necessary, duplicate i.
         if (i->next && !intersect(p,i->next, &zz)) {
            i->next->prev = new arc(i->p,i,i->next);
            i->next = i->next->prev;
         }
         else i->next = new arc(i->p,i);
         i->next->s1 = i->s1;

         // Add p between i and i->next.
         i->next->prev = new arc(p,i,i->next);
         i->next = i->next->prev;

         i = i->next; // Now i points to the new arc.

         // Add new half-edges connected to i's endpoints.
         i->prev->s1 = i->s0 = new seg(z);
         i->next->s0 = i->s1 = new seg(z);

         // Check for new circle events around the new arc:
         check_circle_event(i, p.x);
         check_circle_event(i->prev, p.x);
         check_circle_event(i->next, p.x);

         return;
      }
   }

   // Special case: If p never intersects an arc, append it to the list.
   arc *i;
   for (i = root; i->next; i=i->next) ; // Find the last node.

   i->next = new arc(p,i);
   // Insert segment between p and i
   point start;
   start.x = X0;
   start.y = (i->next->p.y + i->p.y) / 2;
   i->s1 = i->next->s0 = new seg(start);
}

```



## 1.4 root

**starting Point**: It's the starting point of the linked list. When you iterate through the beach line, you'd typically start from `root` and follow the `next` pointers to reach subsequent `arc`s in the sequence.





# 2. parabola's appearance & disappearance



## 2.1 **Site Events**

These occur when a new **parabola** (associated with a site) is **added** to the "**beach line**" or the "parabolic front". It is called a site event because the event is centered around a specific site (point) being processed.



## 2.2 Circle Events

These occur when a **parabola** (associated with a site) **disappears** from the "beach line" due to it being "**squeezed out**" by its neighboring parabolas. This happens at the exact point in the sweep process where the parabola's focus point (the site) is on the circumference of a circle whose edge is tangent to its neighboring parabolas.



### 2.2.1 the procedure of "being squeezed out"



![](https://en.wikipedia.org/wiki/File:Fortunes-algorithm.gif)

1. As the sweep line moves, the current parabolas on the beach line change shape and position.
2. When a new point is encountered, a new parabola begins to grow.
3. https://jacquesheunis.com/post/fortunes-algorithm/



### 2.2.2 process circle events

1. **Identifying a potential circle event**

```cpp
check_circle_event()
```

2. **Determine if a circle event should occur**

```cpp
circle()
```

The function `circle` checks if a valid circle can be formed with three points. 







Once a cell has been completely surrounded by other cells, it obviously cannot grow any further



















