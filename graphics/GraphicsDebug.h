#ifndef GRAPHICSDEBUG_H
#define GRAPHICSDEBUG_H
#ifdef DEBUG
#define DPRINTF printf("%s:%d ",__FILE__,__LINE__),printf
#define DEBUGE(a) printf("%s:%d %s\n",__FILE__,__LINE__,#a),a
#else
#define DPRINTF void
#define DEBUGE(a) a
#endif
#endif
