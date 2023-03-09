#include <SFML/Graphics.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "all.h"

float dist(point p1, point p2) {
    return hypotf(p1.x - p2.x, p1.y - p2.y);
};
float norm(point p) {
    return hypotf(p.x, p.y);
};
float scal(point p1, point p2) {
    return p1.x*p2.x + p1.y*p2.y;
};
point pt_add(point p1, point p2) {
    point out;

    out.x = p1.x + p2.x;
    out.y = p1.y + p2.y;
    return out;
};
point homot(point p, float k) {
    point out;

    out.x = p.x * k;
    out.y = p.y * k;
    return out;
};
point pt_sub(point p1, point p2) {
    point out;
    
    out.x = p1.x -p2.x;
    out.y = p1.y -p2.y;
    return out;
}
point rotate_left(point in) {
    point out;
    
    out.x = -in.y;
    out.y = in.x;
    return out;
};
point unit_vect(point in) {
    point out;
    float r;
    
    r = hypotf(in.x,in.y);
    out.x = in.x/r;
    out.y = in.y/r;
    return out;
}
///
point edge_end(edge* in) {
    point out;
    
    out.x = in->a.x + (in->u.x)*(in->l);
    out.y = in->a.y + (in->u.y)*(in->l);
    return out;
};
edge* clone_edge(edge* e){
    edge* e2;
    
    e2 = malloc(sizeof(edge));
    if(!e2) return NULL;
    *e2 = *e;
    return e2;
};
edge* new_edge(void) {
    edge* e;
    
    e = (edge*)malloc(sizeof(edge));
    return e;
};
point intersect_edge(edge* e1, edge* e2, int* ok){
    point out;
    point ortho;
    float pp;
    float qq;
    float hh;

    ortho = rotate_left(e1->u);
    pp = scal(pt_sub(e2->a,e1->a),ortho);
    qq = scal(pt_sub(edge_end(e2),e1->a),ortho);
    if(pp*qq>0){*ok = 0; return bad_pt;};
    if(pp<0){pp = -pp;}
    if(qq<0){qq = -qq;}
    out = pt_add(e2->a,homot(e2->u,(e2->l)*(pp/(pp+qq))));
    hh = scal(pt_sub(out,e1->a),e1->u);
    if(hh<0 || hh>e1->l) {*ok = 0; return bad_pt;};
    *ok = 1;
    return out;
};
///
float vect_prod(point p1, point p2) {
    return p1.x*p2.y - p1.y*p2.x;
};
int is_on_right(edge* e, point p) {
    point ap;
    
    ap = pt_sub(p,e->a);
    if (vect_prod(e->u,ap)>0) return 0;
    return 1;
};
wall* clone_wall(wall* w){
    wall* w2;
    chain* l;
    chain* c;
    int first;

    w2 = (wall*)malloc(sizeof(wall));
    if(!w2) return NULL;
    l = NULL;
    first = 1;
    for(c=w->edges;(c!=w->edges || first);c = c->next){
        chain* c2;

        first = 0;
        c2 = new_chain();
        c2->payload = (void*)clone_edge((edge*)c->payload);
        c2->next = l;
        l = c2;
    };
    cycle_chain(l);
    w2->edges = l;
    return w2;
};
chain* triangle_decomp(wall* w){
    chain* l;
    chain* tri;
    chain* c;

    w = clone_wall(w);
    l = NULL;
    while(1){
        tri = triangle_pop(w);
        if (!tri) break;
        c = new_chain();
        c->payload = (void*)tri;
        c->next = l;
        l = c;
    }
    c = new_chain();
    c->payload=(void*)w->edges;
    c->next = l;
    l = c;
    return l;
};
int trivial(chain* e){
    if (e->next==e || e->next->next == e || e->next->next->next == e) return 1;
    return 0;
};

chain* triangle_pop(wall* w) {
    int first;
    chain* curr;

    if (trivial(w->edges)) return NULL;
    first = 1;
    for(curr = w->edges;(curr != w->edges || first);curr = curr->next){ // invariant: w contient ce qu'il reste à decomposer
        edge* pe1;
        edge* pe2;
        edge e3;
        point p;
        point pa;
        int valid;
        chain* curr2;

        pe1 = (edge*)curr->payload;
        pe2 = (edge*)(curr->next)->payload;
        p = edge_end(pe2);
        if (is_on_right(pe1,p)) continue; 
        e3.a = p;
        pa = pt_sub(pe1->a,p);
        e3.u = unit_vect(pa);
        e3.l = norm(pa);
        valid = 1;
        for (curr2 = curr->next->next;curr2 != curr;curr2=curr2->next){
            int ok;

            intersect_edge(&e3,(edge*)curr2->payload,&ok);
            if (ok) {valid = 0; break;}
        }
        if (valid){
            edge* pe3;
            edge* ppe3;
            chain* c1;
            chain* c2;
            chain* c3;
            chain* tri;
            chain* dead;
            
            pe3 = clone_edge(&e3);
            c1 = new_chain();
            c2 = new_chain();
            c3 = new_chain();
            c1->next = c2;
            c2->next = c3;
            c3->next = c1;
            c1->payload = (void*)pe1;
            c2->payload = (void*)pe2;
            c3->payload = (void*)pe3;
            tri = c1;
            e3.u = homot(e3.u,-1);
            e3.a = pe1->a;
            ppe3 = clone_edge(&e3);
            curr->payload = (void*)ppe3;
            dead = curr->next;
            curr->next = curr->next->next;
            free(dead);
            return tri;
        }
    }
    printf("Théorème faux\n");
    exit(-1);
};
///
chain* new_chain(void) {
    chain* c;

    c = (chain*)malloc(sizeof(chain));
    if (!c) return NULL;
    c->payload = NULL;
    c->next = NULL;
    return c;
};
void cycle_chain(chain* c){
    chain* b;
    chain* d;
    
    b = c;
    d = NULL;
    for(;c;c=c->next){
        d = c;
    }
    if(d) {
        d->next = b;
    }
};

int main() {
/*     edge e1;
    point ae1;
    point ue1;
    float le1;
    ae1.x = 0;
    ae1.y = 0;
    ue1.x = 1;
    ue1.y = 1;
    ue1 = unit_vect(ue1);
    le1 = 3;
    e1.a = ae1;
    e1.u = ue1;
    e1.l = le1;

    edge e2;
    point ae2;
    point ue2;
    float le2;
    ae2.x = 0;
    ae2.y = 3;
    ue2.x = 1;
    ue2.y = -1;
    ue2 = unit_vect(ue2);
    le2 = 3;
    e2.a = ae2;
    e2.u = ue2;
    e2.l = le2;

    //expected = 1.5;1.5

    
    int res;
    point sect = intersect_edge(&e1,&e2,&res);
    printf("res = %d, x = %f, y = %f\n",res,sect.x,sect.y); */
    edge e;
    e.a.x = 0;
    e.a.y = 0;
    e.u.x = 0;
    e.u.y = 1;
    e.l = 2;
    const point pt = {1,1};
    int res = is_on_right(&e,pt);
    printf("res = %d\n",res);

}