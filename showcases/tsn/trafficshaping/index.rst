Scheduling and Traffic Shaping
==============================

Traffic scheduling and shaping aim to achieve bounded low latency and zero
congestion loss. Scheduling and shaping decisions are made on a per-stream,
per-priority, per-frame, etc. basis using various methods.

**TODO** in interfaces (as opposed to policing)

The following showcases demonstrate scheduling and traffic shaping:

.. toctree::
   :maxdepth: 1

   timeawareshaper/doc/index
   creditbasedshaper/doc/index
   asynchronousshaper/doc/index
   mixingshapers/doc/index
   underthehood/doc/index

.. tokenbucketbasedshaper/doc/index
.. htbshaper/doc/index
