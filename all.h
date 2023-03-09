///utils

typedef struct chain {
    struct chain* next;
    void* payload;
} chain;
chain* new_chain(void);
void cycle_chain(chain* c);

///

typedef struct point {
    float x;
    float y;
} point;
const point bad_pt = {NAN,NAN};

float dist(point p1, point p2);
float scal(point p1, point p2);
point pt_add(point p1, point p2);
point homot(point p, float k);
point pt_sub(point p1, point p2);
point rotate_left(point in);
point unit_vect(point in);
float norm(point p);
float vect_prod(point p1, point p2);

///

typedef struct edge {
    point a;
    //b = a + u*l
    point u;
    float l;
} edge;
int is_on_right(edge* e, point p);
point intersect_edge(edge* e1, edge* e2, int* ok);
point edge_end(edge* in);
edge* clone_edge(edge* e);
edge* new_edge(void);

///

typedef struct wall {
    // la matière est à gauche
    // liste CYCLIQUE de edge
    chain* edges;
} wall;
wall* clone_wall(wall* w);
chain* triangle_decomp(wall* w);
chain* triangle_pop(wall* w);

///

typedef struct ball {
    point pos;
    point v;
    float m;
} ball;



