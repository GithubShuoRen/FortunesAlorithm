//
// Created by ShuoRen on 2023-08-03.
//

#ifndef FORTUNE_VORONOI_H
#define FORTUNE_VORONOI_H


#include <iostream>
#include <queue>
#include <set>
#include <cmath>

using namespace std;

typedef std::pair<double, double> point;


class Voronoi {
private:
    // Notation for working with points

#define x first
#define y second

    struct Arc;
    struct Seg;

    struct Event {
        double x;
        point p;
        Arc *a;
        bool valid;

        Event(double xx, point pp, Arc *aa) : x(xx), p(pp), a(aa), valid(true) {}
    };

    struct Arc {
        point p;
        Arc *prev, *next;
        Event *e;

        Seg *s0, *s1;

        Arc(point pp, Arc *a = 0, Arc *b = 0)
                : p(pp), prev(a), next(b), e(0), s0(0), s1(0) {}
    };

    static vector<Seg*> output;  // Array of output segments.

    struct Seg {
        point start, end;
        bool done;

        Seg(point p) : start(p), end(0, 0), done(false) {
            output.push_back(this);
        }

        // Set the end point and mark as "done."
        void finish(point p) {
            if (done) return;
            end = p;
            done = true;
        }
    };

    Arc *root = nullptr; // First item in the parabolic front linked list.

    // "Greater than" comparison, for reverse sorting in priority queue.
    struct gt {
        bool operator()(point a, point b) { return a.x == b.x ? a.y > b.y : a.x > b.x; }
        bool operator()(Event *a, Event *b) { return a->x > b->x; }
    };

    // Bounding box coordinates.
    double X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

    priority_queue<point, vector<point>, gt> points; // site events
    priority_queue<Event*, vector<Event*>, gt> events; // circle events

    point intersection(point p0, point p1, double l);
    bool intersect(point p, Arc *i, point *res);
    bool circle(point a, point b, point c, double *x, point *o);
    void check_circle_event(Arc *i, double x0);
    void front_insert(point p);
    void process_event();
    void process_point();
    void finish_edges();
    void print_output();

public:
    Voronoi() = default;
    void run();
};

point Voronoi::intersection(point p0, point p1, double l) {
    point res, p = p0;

    if (p0.x == p1.x)
        res.y = (p0.y + p1.y) / 2;
    else if (p1.x == l)
        res.y = p1.y;
    else if (p0.x == l) {
        res.y = p0.y;
        p = p1;
    } else {
        // Use the quadratic formula.
        double z0 = 2*(p0.x - l);
        double z1 = 2*(p1.x - l);

        double a = 1/z0 - 1/z1;
        double b = -2*(p0.y/z0 - p1.y/z1);
        double c = (p0.y*p0.y + p0.x*p0.x - l*l)/z0
                   - (p1.y*p1.y + p1.x*p1.x - l*l)/z1;

        res.y = ( -b - sqrt(b*b - 4*a*c) ) / (2*a);
    }
    // Plug back into one of the parabola equations.
    res.x = (p.x*p.x + (p.y-res.y)*(p.y-res.y) - l*l)/(2*p.x-2*l);
    return res;
}

bool Voronoi::intersect(point p, Arc *i, point *res) {
    if (i->p.x == p.x) return false;

    double a,b;
    if (i->prev) // Get the intersection of i->prev, i.
        a = intersection(i->prev->p, i->p, p.x).y;
    if (i->next) // Get the intersection of i->next, i.
        b = intersection(i->p, i->next->p, p.x).y;

    if ((!i->prev || a <= p.y) && (!i->next || p.y <= b)) {
        res->y = p.y;

        // Plug it back into the parabola equation.
        res->x = (i->p.x*i->p.x + (i->p.y-res->y)*(i->p.y-res->y) - p.x*p.x)
                 / (2*i->p.x - 2*p.x);

        return true;
    }
    return false;
}

bool Voronoi::circle(point a, point b, point c, double *x, point *o) {
    // Check that bc is a "right turn" from ab.
    if ((b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) > 0)
        return false;

    // Algorithm from O'Rourke 2ed p. 189.
    double A = b.x - a.x,  B = b.y - a.y,
            C = c.x - a.x,  D = c.y - a.y,
            E = A*(a.x+b.x) + B*(a.y+b.y),
            F = C*(a.x+c.x) + D*(a.y+c.y),
            G = 2*(A*(c.y-b.y) - B*(c.x-b.x));

    if (G == 0) return false;  // Points are co-linear.

    // Point o is the center of the circle.
    o->x = (D*E-B*F)/G;
    o->y = (A*F-C*E)/G;

    // o.x plus radius equals max x coordinate.
    *x = o->x + sqrt( pow(a.x - o->x, 2) + pow(a.y - o->y, 2) );
    return true;
}

void Voronoi::check_circle_event(Arc *i, double x0) {
    // Invalidate any old Event.
    if (i->e && i->e->x != x0)
        i->e->valid = false;
    i->e = NULL;

    if (!i->prev || !i->next)
        return;

    double x;
    point o;

    if (circle(i->prev->p, i->p, i->next->p, &x,&o) && x > x0) {
        // Create new Event.
        i->e = new Event(x, o, i);
        events.push(i->e);
    }
}

void Voronoi::front_insert(point p) {
    if (!root) {
        root = new Arc(p);
        return;
    }

    // Find the current Arc(s) at height p.y (if there are any).
    for (Arc *i = root; i; i = i->next) {
        point z, zz;
        if (intersect(p,i,&z)) {
            // New parabola intersects Arc i.  If necessary, duplicate i.
            if (i->next && !intersect(p,i->next, &zz)) {
                i->next->prev = new Arc(i->p, i, i->next);
                i->next = i->next->prev;
            }
            else i->next = new Arc(i->p, i);
            i->next->s1 = i->s1;

            // Add p between i and i->next.
            i->next->prev = new Arc(p, i, i->next);
            i->next = i->next->prev;

            i = i->next; // Now i points to the new Arc.

            // Add new half-edges connected to i's endpoints.
            i->prev->s1 = i->s0 = new Seg(z);
            i->next->s0 = i->s1 = new Seg(z);

            // Check for new circle events around the new Arc:
            check_circle_event(i, p.x);
            check_circle_event(i->prev, p.x);
            check_circle_event(i->next, p.x);

            return;
        }
    }

    // Special case: If p never intersects an Arc, append it to the list.
    Arc *i;
    for (i = root; i->next; i=i->next) ; // Find the last node.

    i->next = new Arc(p, i);
    // Insert segment between p and i
    point start;
    start.x = X0;
    start.y = (i->next->p.y + i->p.y) / 2;
    i->s1 = i->next->s0 = new Seg(start);
}

void Voronoi::process_event() {
    // Get the next Event from the queue.
    Event *e = events.top();
    events.pop();

    if (e->valid) {
        // Start a new edge.
        Seg *s = new Seg(e->p);

        // Remove the associated Arc from the front.
        Arc *a = e->a;
        if (a->prev) {
            a->prev->next = a->next;
            a->prev->s1 = s;
        }
        if (a->next) {
            a->next->prev = a->prev;
            a->next->s0 = s;
        }

        // Finish the edges before and after a.
        if (a->s0) a->s0->finish(e->p);
        if (a->s1) a->s1->finish(e->p);

        // Recheck circle events on either side of p:
        if (a->prev) check_circle_event(a->prev, e->x);
        if (a->next) check_circle_event(a->next, e->x);
    }
    delete e;
}

void Voronoi::process_point() {
    // Get the next point from the queue.
    point p = points.top();
    points.pop();

    // Add a new Arc to the parabolic front.
    front_insert(p);
}

void Voronoi::finish_edges() {
    // Advance the sweep line so no parabolas can cross the bounding box.
    double l = X1 + (X1-X0) + (Y1-Y0);

    // Extend each remaining segment to the new parabola intersections.
    for (Arc *i = root; i->next; i = i->next)
        if (i->s1)
            i->s1->finish(intersection(i->p, i->next->p, l*2));
}

void Voronoi::print_output() {
    // Bounding box coordinates.
    cout << X0 << " "<< X1 << " " << Y0 << " " << Y1 << endl;

    // Each output segment in four-column format.
    vector<Seg*>::iterator i;
    for (i = output.begin(); i != output.end(); i++) {
        point p0 = (*i)->start;
        point p1 = (*i)->end;
        cout << p0.x << " " << p0.y << " " << p1.x << " " << p1.y << endl;
    }
}

void Voronoi::run() {
    point p;
    while (cin >> p.x >> p.y) {
        points.push(p);
        if (p.x < X0) X0 = p.x;
        if (p.y < Y0) Y0 = p.y;
        if (p.x > X1) X1 = p.x;
        if (p.y > Y1) Y1 = p.y;
    }
    double dx = (X1 - X0 + 1) / 5.0, dy = (Y1 - Y0 + 1) / 5.0;
    X0 -= dx;  X1 += dx;  Y0 -= dy;  Y1 += dy;

    while (!points.empty()) {
        if (!events.empty() && events.top()->x <= points.top().x) {
            process_event();
        } else {
            process_point();
        }
    }

    while (!events.empty()) {
        process_event();
    }

    finish_edges();
    print_output();
}


#endif //FORTUNE_VORONOI_H
