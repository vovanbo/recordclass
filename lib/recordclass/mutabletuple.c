/* mutabletuple type *********************************************************/

#include "Python.h"
#include "structmember.h"

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

#if PY_MAJOR_VERSION >= 3
    #define MOD_DEF(ob, name, doc, methods) \
        static struct PyModuleDef moduledef = { \
            PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
        ob = PyModule_Create(&moduledef);
#else
    #define MOD_DEF(ob, name, doc, methods) \
        ob = Py_InitModule3(name, methods, doc);
#endif

static PyTypeObject PyMutableTuple_Type; /* Forward */
static PyObject * mutabletuple_slice(PyObject *a, Py_ssize_t ilow, Py_ssize_t ihigh);


PyObject *
PyMutableTuple_New(Py_ssize_t size)
{
    PyTupleObject *op;
    Py_ssize_t i;
    if (size < 0) {
        PyErr_BadInternalCall();
        return NULL;
    }
    {
        /* Check for overflow */
        if (size > (PY_SSIZE_T_MAX - sizeof(PyTupleObject) -
                    sizeof(PyObject *)) / sizeof(PyObject *)) {
            return PyErr_NoMemory();
        }
        
        op = PyObject_GC_NewVar(PyTupleObject, &PyMutableTuple_Type, size);
        if (op == NULL)
            return NULL;
    }
    
    for (i=0; i < size; i++)
        op->ob_item[i] = NULL;
    PyObject_GC_Track(op);
    
    return (PyObject *) op;
}

/*
PyDoc_STRVAR(mutabletuple_copy_doc, "D.copy() -> a shallow copy of D.");
*/

static PyObject *
mutabletuple_copy(PyObject *dd)
{
    return mutabletuple_slice(dd, 0, Py_SIZE(dd));
}


PyDoc_STRVAR(reduce_doc, "D.__reduce__()");

static PyObject *
mutabletuple_reduce(PyObject *dd)
{
    PyObject *args;
    PyObject *result;
    PyObject *tmp;
    Py_ssize_t i, n;
        
    n = PyTuple_GET_SIZE(dd);
    tmp = PyTuple_New(n);
    if (tmp == NULL)
        return NULL;
    
    for (i=0; i<n; i++) {
        PyObject *v = PyTuple_GET_ITEM(dd, i);
        Py_INCREF(v);
        PyTuple_SET_ITEM(tmp, i, v);
    }
    
    args = PyTuple_Pack(1, tmp);
    if (args == NULL)
        return NULL;

    result = PyTuple_Pack(2, &PyMutableTuple_Type, args);
    Py_DECREF(args);
    return result;
}


static PyObject *
mutabletuple_repr(PyObject *dd)
{
    PyObject *baserepr;
    PyObject *result;
    PyObject *lst, *v;
    Py_ssize_t i, n;
    
    n = PyTuple_GET_SIZE(dd);
    lst = PyList_New(n);
    if (lst == NULL)
        return NULL;
        
    for (i=0; i<n; i++) {
        v = PyTuple_GET_ITEM(dd, i);
        Py_INCREF(v);
        PyList_SET_ITEM(lst, i, v);
    }
        
    baserepr = PyList_Type.tp_repr(lst);
    if (baserepr == NULL)
        return NULL;

#if PY_MAJOR_VERSION >= 3
    result = PyUnicode_FromFormat("mutabletuple(%U)", baserepr);
#else
    result = PyString_FromFormat("mutabletuple(%s)", PyString_AS_STRING(baserepr));
#endif
    Py_DECREF(baserepr);
    return result;
}

PyDoc_STRVAR(mutabletuple_doc,
"mutabletuple(...]) --> mutabletuple\n\n\
");

static PyObject *
mutabletuple_concat(PyTupleObject *a, PyObject *bb)
{
    Py_ssize_t size;
    Py_ssize_t i;
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
    np = (PyTupleObject *) PyMutableTuple_New(size);
    if (np == NULL) {
        return NULL;
    }
    
    src = a->ob_item;
    dest = np->ob_item;    
    for (i = 0; i < Py_SIZE(a); i++) {
        PyObject *v = src[i];
        Py_INCREF(v);
        dest[i] = v;
    }
    
    src = b->ob_item;
    dest = np->ob_item + Py_SIZE(a);
    for (i = 0; i < Py_SIZE(b); i++) {
        PyObject *v = src[i];
        Py_INCREF(v);
        dest[i] = v;
    }
#undef b

    return (PyObject *)np;
}

static PyObject *
mutabletuple_slice(PyObject *a, Py_ssize_t ilow,
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
    np = (PyTupleObject *)PyMutableTuple_New(len);
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
mutabletuple_ass_slice(PyTupleObject *a, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v)
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
        if (a == (PyTupleObject*)v) {
            v = mutabletuple_copy(v);
            if (v == NULL)
                return result;
                
            result = mutabletuple_ass_slice(a, ilow, ihigh, v);
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
    
    item = a->ob_item;
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
mutabletuple_ass_item(PyTupleObject *a, Py_ssize_t i, PyObject *v)
{
    PyObject *old_value;
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError,
                        "mutabletuple assignment index out of range");
        return -1;
    }
    if (v == NULL)
        return mutabletuple_ass_slice(a, i, i+1, v);
    Py_INCREF(v);
    old_value = a->ob_item[i];
    a->ob_item[i] = v;
    Py_DECREF(old_value);
    return 0;
}

static PyObject *
mutabletuple_item(PyTupleObject *a, Py_ssize_t i)
{
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError, "mutabletuple index out of range");
        return NULL;
    }
    Py_INCREF(a->ob_item[i]);
    return a->ob_item[i];
}

static PyObject*
mutabletuple_subscript(PyTupleObject* self, PyObject* item)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return NULL;
        if (i < 0)
            i += PyTuple_GET_SIZE(self);
        return mutabletuple_item(self, i);
    }
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength, cur, i;
        PyObject* result;
        PyObject* it;
        PyObject **src, **dest;

#if PY_MAJOR_VERSION >= 3
        if (PySlice_GetIndicesEx(item,
#else
        if (PySlice_GetIndicesEx((PySliceObject*)item,
#endif
                         PyTuple_GET_SIZE(self),
                         &start, &stop, &step, &slicelength) < 0) {
            return NULL;
        }

        if (slicelength <= 0) {
            return PyMutableTuple_New(0);
        }
        else if (start == 0 && step == 1 &&
                 slicelength == PyTuple_GET_SIZE(self) &&
                 PyTuple_CheckExact(self)) {
            Py_INCREF(self);
            return (PyObject *)self;
        }
        else {
            result = PyMutableTuple_New(slicelength);
            if (!result) return NULL;

            src = self->ob_item;
            dest = ((PyTupleObject *)result)->ob_item;
            for (cur = start, i = 0; i < slicelength;
                 cur += step, i++) {
                it = src[cur];
                Py_INCREF(it);
                dest[i] = it;
            }

            return result;
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "tuple indices must be integers, not %.200s",
                     Py_TYPE(item)->tp_name);
        return NULL;
    }
}

static int
mutabletuple_ass_subscript(PyTupleObject* self, PyObject* item, PyObject* value)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return -1;
        if (i < 0)
            i += PyList_GET_SIZE(self);
        return mutabletuple_ass_item(self, i, value);
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

        if (step == 1)
            return mutabletuple_ass_slice(self, start, stop, value);

        /* Make sure s[5:2] = [..] inserts at the right place:
           before 5, not before 2. */
        if ((step < 0 && start < stop) ||
            (step > 0 && start > stop))
            stop = start;

        if (value == NULL) {
            return -1;
        }
        else {
            /* assign slice */
            PyObject *ins, *seq;
            PyObject **seqitems, **selfitems;
            Py_ssize_t cur, i;

            /* protect against a[::-1] = a */
            if (self == (PyTupleObject*)value) {
                seq = mutabletuple_slice(value, 0,
                                   PyTuple_GET_SIZE(value));
            }
            else {
                seq = PySequence_Fast(value,
                                      "must assign iterable "
                                      "to extended slice");
            }
            if (!seq)
                return -1;

            if (PySequence_Fast_GET_SIZE(seq) != slicelength) {
                PyErr_Format(PyExc_ValueError,
                    "attempt to assign sequence of "
                    "size %zd to extended slice of "
                    "size %zd",
                         PySequence_Fast_GET_SIZE(seq),
                         slicelength);
                Py_DECREF(seq);
                return -1;
            }

            if (!slicelength) {
                Py_DECREF(seq);
                return 0;
            }

            selfitems = self->ob_item;
            seqitems = PySequence_Fast_ITEMS(seq);
            for (cur = start, i = 0; i < slicelength;
                 cur += (size_t)step, i++) {
                PyObject *u = selfitems[cur];
                ins = seqitems[i];
                Py_INCREF(ins);
                selfitems[cur] = ins;
                Py_DECREF(u);
            }

            Py_DECREF(seq);

            return 0;
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "mutabletuple indices must be integers, not %.200s",
                     item->ob_type->tp_name);
        return -1;
    }
}

static PyObject *
mutabletuple_repeat(PyTupleObject *a, Py_ssize_t n)
{
    Py_ssize_t i, j;
    Py_ssize_t size;
    PyTupleObject *np;
    PyObject **p, **items;
    if (n < 0)
        n = 0;
    if (Py_SIZE(a) == 0) {
        return PyMutableTuple_New(0);
    }
    if (n > PY_SSIZE_T_MAX / Py_SIZE(a))
        return PyErr_NoMemory();
    size = Py_SIZE(a);
    np = (PyTupleObject *) PyMutableTuple_New(Py_SIZE(a) * n);
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
// mutabletuple_getnewargs(PyTupleObject *v)
// {
//     return Py_BuildValue("(N)", mutabletuple_slice(v, 0, Py_SIZE(v)));
// 
// }

PyDoc_STRVAR(sizeof_doc,
"T.__sizeof__() -- size of T in memory, in bytes");

static PyObject *
mutabletuple_sizeof(PyTupleObject *self)
{
    Py_ssize_t res;

    res = PyTuple_Type.tp_basicsize + Py_SIZE(self) * sizeof(PyObject *);
    return PyLong_FromSsize_t(res);
}

static PyObject *
mutabletuple_richcompare(PyObject *v, PyObject *w, int op)
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

static PySequenceMethods mutabletuple_as_sequence = {
    0,                       /* sq_length */
    (binaryfunc)mutabletuple_concat,                    /* sq_concat */
    (ssizeargfunc)mutabletuple_repeat,                  /* sq_repeat */
    /*(ssizeargfunc)mutabletuple_item,*/                    /* sq_item */
    0,                    /* sq_item */
    /*mutabletuple_slice,*/                                          /* sq_slice */
    0,                                          /* sq_slice */
    /*(ssizeobjargproc)mutabletuple_ass_item,*/             /* sq_ass_item */
    0,             /* sq_ass_item */
    0,                                          /* sq_ass_slice */
    0,                  /* sq_contains */
};

static PyMappingMethods mutabletuple_as_mapping = {
    0,
    (binaryfunc)mutabletuple_subscript,
    (objobjargproc)mutabletuple_ass_subscript
};

static PyMethodDef mutabletuple_methods[] = {
    /*{"copy", (PyCFunction)mutabletuple_copy, METH_NOARGS, mutabletuple_copy_doc},*/
    /*{"__copy__", (PyCFunction)mutabletuple_copy, METH_NOARGS, mutabletuple_copy_doc},*/
    /*{"__getnewargs__",  (PyCFunction)mutabletuple_getnewargs,  METH_NOARGS},*/
    {"__sizeof__",      (PyCFunction)mutabletuple_sizeof, METH_NOARGS, sizeof_doc},     
    {"__reduce__", (PyCFunction)mutabletuple_reduce, METH_NOARGS, reduce_doc},
    {NULL}
};

/*
static PyMemberDef mutabletuple_members[] = {
    {"default_factory", T_OBJECT,
     offsetof(mutabletupleobject, default_factory), 0,
     PyDoc_STR("Factory for default value called by __missing__().")},
    {NULL}
};
*/


/* See comment in xxsubtype.c */
#define DEFERRED_ADDRESS(ADDR) 0

static PyTypeObject PyMutableTuple_Type = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "recordarray.mutabletuple.mutabletuple",          /* tp_name */
    sizeof(PyTupleObject),              /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,        /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    (reprfunc)mutabletuple_repr,               /* tp_repr */
    0,                                  /* tp_as_number */
    &mutabletuple_as_sequence,                 /* tp_as_sequence */
    &mutabletuple_as_mapping,                  /* tp_as_mapping */
    PyObject_HashNotImplemented,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
    Py_TPFLAGS_BASETYPE | Py_TPFLAGS_TUPLE_SUBCLASS,                                    /* tp_flags */
    mutabletuple_doc,                        /* tp_doc */
    0,                   /* tp_traverse */
    0,          /* tp_clear */
    mutabletuple_richcompare,                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset*/
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    mutabletuple_methods,                    /* tp_methods */
    0,                    /* tp_members */
    0,                                  /* tp_getset */
    DEFERRED_ADDRESS(&PyTuple_Type),     /* tp_base */
    0,                                  /* tp_mutabletuple */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_mutabletupleoffset */
    0,                       /* tp_init */
    0,                /* tp_alloc */
    0,                                  /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};


/* List of functions defined in the module */


PyDoc_STRVAR(mutabletuplemodule_doc,
"This is a template module just for instruction.");

/* Initialization function for the module (*must* be called PyInit_xx) */

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef mutabletuplemodule = {
    PyModuleDef_HEAD_INIT,
    "recordarray.mutabletuple",
    mutabletuplemodule_doc,
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif

#if PY_MAJOR_VERSION >= 3
    #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
#else
    #define MOD_INIT(name) PyMODINIT_FUNC init##name(void)
#endif

#if PY_MAJOR_VERSION >= 3
    #define PYMOD_RETURN(V) return V;
#else
    #define PYMOD_RETURN(V) return;
#endif


MOD_INIT(mutabletuple)
{
    PyObject *m;

    PyMutableTuple_Type.tp_base = &PyTuple_Type;
    if (PyType_Ready(&PyMutableTuple_Type) < 0)
        PYMOD_RETURN(NULL)

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&mutabletuplemodule);
#else
    m = Py_InitModule3("recordarray.mutabletuple", NULL, mutabletuplemodule_doc);
#endif    
    /*m = PyModule_Create(&mutabletuplemodule);*/
    if (m == NULL)
        PYMOD_RETURN(NULL)


    Py_INCREF(&PyMutableTuple_Type);
    if (PyModule_AddObject(m, "mutabletuple", (PyObject*) &PyMutableTuple_Type) < 0)
        PYMOD_RETURN(NULL)

    PYMOD_RETURN(m)
}
