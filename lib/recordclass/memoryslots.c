/* memoryslots type *********************************************************/

#include "Python.h"

#ifndef Py_RETURN_NOTIMPLEMENTED
/* Macro for returning Py_NotImplemented from a function */
#define Py_RETURN_NOTIMPLEMENTED \
    return Py_INCREF(Py_NotImplemented), Py_NotImplemented
#endif

#ifndef PyModuleDef_HEAD_INIT
#define PyModuleDef_HEAD_INIT { \
    PyObject_HEAD_INIT(NULL)    \
    NULL, /* m_init */          \
    0,    /* m_index */         \
    NULL, /* m_copy */          \
  }
#endif

static PyTypeObject PyMemorySlots_Type;
static PyObject * memoryslots_slice(PyObject *a, Py_ssize_t ilow, Py_ssize_t ihigh);


PyObject *
PyMemorySlots_New(Py_ssize_t size)
{
    PyTupleObject *op;
    Py_ssize_t i;
    if (size < 0) {
        PyErr_BadInternalCall();
        return NULL;
    }
        
    op = PyObject_GC_NewVar(PyTupleObject, &PyMemorySlots_Type, size);
    if (op == NULL)
        return NULL;

    if (size > 0) {
        for (i=0; i < size; i++)
            op->ob_item[i] = NULL;
    }
    PyObject_GC_Track(op);
    
    return (PyObject*)op;
}


static PyObject *
memoryslots_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{

    if (args == NULL)
        return PyMemorySlots_New(0);
    else {
        PyTupleObject *tmp, *newobj;
        Py_ssize_t i, n;
        PyObject *item;
        
        tmp = (PyTupleObject*)PySequence_Tuple(args);
        n = PyTuple_GET_SIZE(tmp);
        newobj = (PyTupleObject*)type->tp_alloc(type, n);
        if (newobj == NULL)
            return NULL;
        if (n > 0) {
            for(i = 0; i < n; i++) {
                item = PyTuple_GET_ITEM(tmp, i);
                Py_INCREF(item);
                PyTuple_SET_ITEM(newobj, i, item);
            }
        }
        Py_DECREF(tmp);
        return (PyObject*)newobj;
    }
}

static void
memoryslots_dealloc(PyTupleObject *op)
{
    Py_ssize_t i;
    Py_ssize_t len =  Py_SIZE(op);
    
    PyObject_GC_UnTrack(op);
    /*Py_TRASHCAN_SAFE_BEGIN(op)*/
    if (len > 0) {
        i = len;
        while (--i >= 0)
            Py_XDECREF(op->ob_item[i]);
    }
    Py_TYPE(op)->tp_free((PyObject *)op);
    /*Py_TRASHCAN_SAFE_END(op)*/
}

static int
memoryslots_traverse(PyTupleObject *o, visitproc visit, void *arg)
{
    Py_ssize_t i;

    for (i = Py_SIZE(o); --i >= 0; )
        Py_VISIT(o->ob_item[i]);
    return 0;
}


/*
PyDoc_STRVAR(memoryslots_copy_doc, "D.copy() -> a shallow copy of D.");
*/

static PyObject *
memoryslots_copy(PyObject *ob)
{
    return memoryslots_slice(ob, 0, Py_SIZE(ob));
}

PyDoc_STRVAR(reduce_doc, "D.__reduce__()");

static PyObject *
memoryslots_reduce(PyObject *ob)
{
    PyObject *args;
    PyObject *result;

    args = PyTuple_Pack(1, ob);
    if (args == NULL)
        return NULL;

    result = PyTuple_Pack(2, &PyMemorySlots_Type, args);
    Py_DECREF(args);
    return result;
}

static PyObject *
memoryslots_repr(PyObject *dd)
{
    PyObject *baserepr;
    PyObject *v, *result;
    Py_ssize_t n;
        
    n = PyTuple_GET_SIZE(dd);

    if (n == 0) {
#if PY_MAJOR_VERSION >= 3
        result = PyUnicode_FromString("memoryslots()\0");
#else
        result = PyString_FromString("memoryslots()\0");
#endif
        return result;
    }

    if (n == 1) {
        v = PyTuple_GET_ITEM(dd, 0);
        baserepr = PyObject_Repr(v);
#if PY_MAJOR_VERSION >= 3
        result = PyUnicode_FromFormat("memoryslots(%U)", baserepr);
#else
        result = PyString_FromFormat("memoryslots(%s)", PyString_AS_STRING(baserepr));
#endif
        return result;
    }    
    
    baserepr = PyTuple_Type.tp_repr(dd);
    if (baserepr == NULL)
        return NULL;

#if PY_MAJOR_VERSION >= 3
    result = PyUnicode_FromFormat("memoryslots%U", baserepr);
#else
    result = PyString_FromFormat("memoryslots%s", PyString_AS_STRING(baserepr));
#endif
    Py_DECREF(baserepr);
    return result;
}

PyDoc_STRVAR(memoryslots_doc,
"memoryslots([...]) --> memoryslots\n\n\
");

static PyObject *
memoryslots_concat(PyTupleObject *a, PyObject *bb)
{
    Py_ssize_t size;
    Py_ssize_t i, n;
    PyObject **src, **dest;
    PyTupleObject *np;
    
    if (!PyTuple_Check(bb)) {
        PyErr_Format(PyExc_TypeError,
             "can only concatenate tuple (not \"%.200s\") to tuple",
                 Py_TYPE(bb)->tp_name);
        return NULL;
    }
#define b ((PyTupleObject *)bb)
    size = Py_SIZE(a) + Py_SIZE(b);
    if (size < 0)
        return PyErr_NoMemory();

    np = (PyTupleObject *) PyMemorySlots_New(size);
    if (np == NULL) {
        return NULL;
    }
    
    src = a->ob_item;
    dest = np->ob_item;

    n = Py_SIZE(a);
    for (i = 0; i < n; i++) {
        PyObject *v = src[i];
        Py_INCREF(v);
        dest[i] = v;
    }
    
    src = b->ob_item;
    dest = np->ob_item + Py_SIZE(a);
    n = Py_SIZE(b);
    for (i = 0; i < n; i++) {
        PyObject *v = src[i];
        Py_INCREF(v);
        dest[i] = v;
    }
#undef b

    return (PyObject *)np;
}

static PyObject *
memoryslots_slice(PyObject *a, Py_ssize_t ilow,
           Py_ssize_t ihigh)
{
#define aa ((PyTupleObject*)a)
    PyTupleObject *np;
    PyObject **src, **dest;
    Py_ssize_t i;
    Py_ssize_t len;
    
    if (ilow < 0)
        ilow = 0;
    if (ihigh > Py_SIZE(a))
        ihigh = Py_SIZE(a);
    if (ihigh < ilow)
        ihigh = ilow;
    if (ilow == 0 && ihigh == Py_SIZE(a) && PyTuple_CheckExact(a)) {
        Py_INCREF(a);
        return a;
    }
    len = ihigh - ilow;
    np = (PyTupleObject *)PyMemorySlots_New(len);
    if (np == NULL)
        return NULL;
    src = aa->ob_item + ilow;
    dest = np->ob_item;
    for (i = 0; i < len; i++) {
        PyObject *v = src[i];
        Py_INCREF(v);
        dest[i] = v;
    }
    return (PyObject *)np;
#undef aa
}

static int
memoryslots_ass_slice(PyObject *a, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v)
{
    PyObject **item;
    PyObject **vitem = NULL;
    PyObject *v_as_SF = NULL; /* PySequence_Fast(v) */
    Py_ssize_t n; /* # of elements in replacement list */
    Py_ssize_t k;
    int result = -1;
    
    if (v == NULL)
        return result;
    else {
        if (a == v) {
            v = memoryslots_copy(v);
            if (v == NULL)
                return result;
                
            result = memoryslots_ass_slice(a, ilow, ihigh, v);
            Py_DECREF(v);
            return result;
        }
        v_as_SF = PySequence_Fast(v, "can only assign an iterable");
        if(v_as_SF == NULL) {
            return result;
        }
        n = PySequence_Fast_GET_SIZE(v_as_SF);
        vitem = PySequence_Fast_ITEMS(v_as_SF);
    }
    
    if (ilow < 0)
        ilow = 0;
    else if (ilow > Py_SIZE(a))
        ilow = Py_SIZE(a);

    if (ihigh < ilow)
        ihigh = ilow;
    else if (ihigh > Py_SIZE(a))
        ihigh = Py_SIZE(a);

    if (n != ihigh - ilow) {
        Py_XDECREF(v_as_SF);    
        return -1;
    }
    
    item = ((PyTupleObject*)a)->ob_item;
    for (k = 0; k < n; k++, ilow++) {
        PyObject *w = vitem[k];
        PyObject *u = item[ilow];
        Py_XDECREF(u);
        item[ilow] = w;
        Py_XINCREF(w);
    }
    Py_XDECREF(v_as_SF);    
    return 0;
}


static int
memoryslots_ass_item(PyObject *a, Py_ssize_t i, PyObject *v)
{
    PyObject *old_value;
    
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError,
                        "assignment index out of range");
        return -1;
    }
    
    if (v == NULL)
        return -1;
        
    Py_INCREF(v);
    old_value = ((PyTupleObject*)a)->ob_item[i];
    ((PyTupleObject*)a)->ob_item[i] = v;
    Py_XDECREF(old_value);
    return 0;
}

static PyObject *
memoryslots_item(PyObject *a, Py_ssize_t i)
{
    PyObject* v;
    
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }
    v = ((PyTupleObject*)a)->ob_item[i];
    Py_INCREF(v);
    return v;
}

static PyObject*
memoryslots_subscript(PyObject* self, PyObject* item)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return NULL;
        if (i < 0)
            i += PyTuple_GET_SIZE(self);
        return memoryslots_item(self, i);
    }
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength;

#if PY_MAJOR_VERSION >= 3
        if (PySlice_GetIndicesEx(item,
#else
        if (PySlice_GetIndicesEx((PySliceObject*)item,
#endif
                         PyTuple_GET_SIZE(self),
                         &start, &stop, &step, &slicelength) < 0) {
            return NULL;
        }
                
        return memoryslots_slice((PyObject*)self, start, stop);
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "indices must be integers, not %.200s",
                     Py_TYPE(item)->tp_name);
        return NULL;
    }
}

static int
memoryslots_ass_subscript(PyObject* self, PyObject* item, PyObject* value)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return -1;
        if (i < 0)
            i += PyList_GET_SIZE(self);
        return memoryslots_ass_item(self, i, value);
    }
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength;

#if PY_MAJOR_VERSION >= 3
        if (PySlice_GetIndicesEx(item, Py_SIZE(self),
#else
        if (PySlice_GetIndicesEx((PySliceObject*)item, Py_SIZE(self),
#endif
                         &start, &stop, &step, &slicelength) < 0) {
            return -1;
        }

        return memoryslots_ass_slice(self, start, stop, value);

    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "indices must be integers, not %.200s",
                     item->ob_type->tp_name);
        return -1;
    }
}

static PyObject *
memoryslots_repeat(PyTupleObject *a, Py_ssize_t n)
{
    Py_ssize_t i, j;
    Py_ssize_t size;
    PyTupleObject *np;
    PyObject **p, **items;
    if (n < 0)
        n = 0;
    if (Py_SIZE(a) == 0) {
        return PyMemorySlots_New(0);
    }
    if (n > PY_SSIZE_T_MAX / Py_SIZE(a))
        return PyErr_NoMemory();
    size = Py_SIZE(a);
    np = (PyTupleObject *) PyMemorySlots_New(Py_SIZE(a) * n);
    if (np == NULL)
        return NULL;
    p = np->ob_item;
    items = a->ob_item;
    for (i = 0; i < n; i++) {
        for (j = 0; j < size; j++) {
            *p = items[j];
            Py_INCREF(*p);
            p++;
        }
    }
    return (PyObject *) np;
}

// static PyObject *
// memoryslots_getnewargs(PyTupleObject *v)
// {
//     return Py_BuildValue("(N)", memoryslots_slice(v, 0, Py_SIZE(v)));
// 
// }

PyDoc_STRVAR(sizeof_doc,
"T.__sizeof__() -- size of T in memory, in bytes");

static PyObject *
memoryslots_sizeof(PyTupleObject *self)
{
    Py_ssize_t res;

    res = PyTuple_Type.tp_basicsize + Py_SIZE(self) * sizeof(PyObject *);
    return PyLong_FromSsize_t(res);
}

static PyObject *
memoryslots_richcompare(PyObject *v, PyObject *w, int op)
{
    PyTupleObject *vt, *wt;
    Py_ssize_t i;
    Py_ssize_t vlen, wlen;

    if (!PyTuple_Check(v) || !PyTuple_Check(w))
        Py_RETURN_NOTIMPLEMENTED;

    vt = (PyTupleObject *)v;
    wt = (PyTupleObject *)w;

    vlen = Py_SIZE(vt);
    wlen = Py_SIZE(wt);
    

    /* Note:  the corresponding code for lists has an "early out" test
     * here when op is EQ or NE and the lengths differ.  That pays there,
     * but Tim was unable to find any real code where EQ/NE tuple
     * compares don't have the same length, so testing for it here would
     * have cost without benefit.
     */

    /* Search for the first index where items are different.
     * Note that because tuples are immutable, it's safe to reuse
     * vlen and wlen across the comparison calls.
     */
    for (i = 0; i < vlen && i < wlen; i++) {
        int k = PyObject_RichCompareBool(vt->ob_item[i],
                                         wt->ob_item[i], Py_EQ);
        if (k < 0)
            return NULL;
        if (!k)
            break;
    }

    if (i >= vlen || i >= wlen) {
        /* No more items to compare -- compare sizes */
        int cmp;
        PyObject *res;
        switch (op) {
        case Py_LT: cmp = vlen <  wlen; break;
        case Py_LE: cmp = vlen <= wlen; break;
        case Py_EQ: cmp = vlen == wlen; break;
        case Py_NE: cmp = vlen != wlen; break;
        case Py_GT: cmp = vlen >  wlen; break;
        case Py_GE: cmp = vlen >= wlen; break;
        default: return NULL; /* cannot happen */
        }
        if (cmp)
            res = Py_True;
        else
            res = Py_False;
        Py_INCREF(res);
        return res;
    }

    /* We have an item that differs -- shortcuts for EQ/NE */
    if (op == Py_EQ) {
        Py_INCREF(Py_False);
        return Py_False;
    }
    if (op == Py_NE) {
        Py_INCREF(Py_True);
        return Py_True;
    }

    /* Compare the final item again using the proper operator */
    return PyObject_RichCompare(vt->ob_item[i], wt->ob_item[i], op);
}

static PySequenceMethods memoryslots_as_sequence = {
    0,                       /* sq_length */
    (binaryfunc)memoryslots_concat,                    /* sq_concat */
    (ssizeargfunc)memoryslots_repeat,                  /* sq_repeat */
    (ssizeargfunc)memoryslots_item,                    /* sq_item */
    0,                                          /* sq_slice */
    (ssizeobjargproc)memoryslots_ass_item,             /* sq_ass_item */
    0,             /* sq_ass_item */
    0,                                          /* sq_ass_slice */
    0,                  /* sq_contains */
};

static PyMappingMethods memoryslots_as_mapping = {
    0,
    (binaryfunc)memoryslots_subscript,
    (objobjargproc)memoryslots_ass_subscript
};

static PyMethodDef memoryslots_methods[] = {
    /*{"copy", (PyCFunction)memoryslots_copy, METH_NOARGS, memoryslots_copy_doc},*/
    /*{"__copy__", (PyCFunction)memoryslots_copy, METH_NOARGS, memoryslots_copy_doc},*/
    /*{"__getnewargs__",  (PyCFunction)memoryslots_getnewargs,  METH_NOARGS},*/
    {"__sizeof__",      (PyCFunction)memoryslots_sizeof, METH_NOARGS, sizeof_doc},     
    {"__reduce__", (PyCFunction)memoryslots_reduce, METH_NOARGS, reduce_doc},
    {NULL}
};

/*
static PyMemberDef memoryslots_members[] = {
    {"default_factory", T_OBJECT,
     offsetof(memoryslotsobject, default_factory), 0,
     PyDoc_STR("Factory for default value called by __missing__().")},
    {NULL}
};
*/


/* See comment in xxsubtype.c */
#define DEFERRED_ADDRESS(ADDR) 0

static PyTypeObject PyMemorySlots_Type = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "recordarray.memoryslots.memoryslots",          /* tp_name */
    sizeof(PyTupleObject),              /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)memoryslots_dealloc,        /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    (reprfunc)memoryslots_repr,               /* tp_repr */
    0,                                  /* tp_as_number */
    &memoryslots_as_sequence,                 /* tp_as_sequence */
    &memoryslots_as_mapping,                  /* tp_as_mapping */
    PyObject_HashNotImplemented,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
    Py_TPFLAGS_BASETYPE,                                    /* tp_flags */
    memoryslots_doc,                        /* tp_doc */
    (traverseproc)memoryslots_traverse,                   /* tp_traverse */
    0,          /* tp_clear */
    memoryslots_richcompare,                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset*/
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    memoryslots_methods,                    /* tp_methods */
    0,                    /* tp_members */
    0,                                  /* tp_getset */
    DEFERRED_ADDRESS(&PyTuple_Type),     /* tp_base */
    0,                                  /* tp_memoryslots */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_memoryslotsoffset */
    0,                       /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    memoryslots_new,                                  /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
    0                                   /* tp_is_gc */
};


/* List of functions defined in the module */


PyDoc_STRVAR(memoryslotsmodule_doc,
"This is a template module just for instruction.");

/* Initialization function for the module (*must* be called PyInit_xx) */

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef memoryslotsmodule = {
    PyModuleDef_HEAD_INIT,
    "recordarray.memoryslots",
    memoryslotsmodule_doc,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_memoryslots(void)
{
    PyObject *m;

    m = PyModule_Create(&memoryslotsmodule);
    if (m == NULL)
        return NULL;

    PyMemorySlots_Type.tp_base = &PyTuple_Type;
    if (PyType_Ready(&PyMemorySlots_Type) < 0)
        return NULL;
    Py_INCREF(&PyMemorySlots_Type);

    PyModule_AddObject(m, "memoryslots", (PyObject *)&PyMemorySlots_Type);

    return m;
}
#else
PyMODINIT_FUNC
initmemoryslots(void)
{
    PyObject *m;

    m = Py_InitModule3("recordclass.memoryslots", NULL, memoryslotsmodule_doc);
    if (m == NULL)
        return;

    PyMemorySlots_Type.tp_base = &PyType_Type;
    if (PyType_Ready(&PyMemorySlots_Type) < 0)
        return;
    Py_INCREF(&PyMemorySlots_Type);

    PyModule_AddObject(m, "memoryslots", (PyObject *)&PyMemorySlots_Type);

    return;
}
#endif
