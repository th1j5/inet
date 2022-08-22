Statistical Policing
====================

Goals
-----

In this example we combine a sliding window rate meter with a probabilistic packet
dropper to achieve a simple statistical policing.

| INET version: ``4.4``
| Source files location: `inet/showcases/tsn/streamfiltering/statistical <https://github.com/inet-framework/inet/tree/master/showcases/tsn/streamfiltering/statistical>`__

The Model
---------

Filtering Overview
~~~~~~~~~~~~~~~~~~

Filtering and policing - we'll use the two terms interchangably here - enforces requirements for data rate and burst size in a packet stream,
by dropping excessive packets from the stream. In INET, by default, filtering is done in the bridging layer of TSN network nodes (however, the
modules can be inserted elsewhere in the network stack; they don't even require a complete network to function, see TODO showcase).

In INET, TSN network nodes, such as TsnDevice or TsnSwitch, have boolean parameters that enable ingress and egress filtering (similarly to parameters
that enable ingress or egress traffic shaping). For example, TsnDevice has :par:`ingressTrafficFiltering` and :par:`egressTrafficFiltering` parameters,
TsnSwitch has :par:`ingressTrafficFiltering` (check the NED documentation for TSN network nodes for the available parameters).
Setting any of the boolean traffic filtering parameters adds a StreamFilteringLayer to the network node's bridging layer. It also adds an ingressFilter
or egressFilter submodule (or both, depending on the direction) that has the type SimpleIeee8021qFilter. This module can handle a configurable
number of traffic streams (numTrafficStreams). contains TODO:

This module has a configurable number of paths, each of which can handle the metering and filtering of a traffic stream independently of the other streams.

This module can handle a configurable number of traffic streams. Each traffic stream has a path that can handle the metering and filtering of the stream independently of the other streams.
For example, here is a SimpleIeee8021qFilter module with two traffic streams:

.. figure:: media/SimpleIeee8021qFilter.png
   :align: center

Some notes:

- By default, the packets for the different streams are classified by a :ned:`StreamClassifier` that classifies packets by stream name. Also,
  the direct, unfiltered path between the classifier and the multiplexer is available by default (:par:`hasDefaultPath`).
- It has a meter submodule, where traffic streams can be metered. The meter submodule adds a tag based on the metering to the packet. The filter module
  then can use this information to drop excessive packets, for example.
- The gate module is an InteractiveGate, that is always open, but can be closed by user interaction.

By overriding the type of the meter and filter submodules, different filtering behaviors can be achieved. For example, the meter could be a 
statisticalmeter that meters data rate of the stream and attached tags and the filter drops them. Some traffic shapers have elements which are
in the filtering layer such as AST. etc. Also, the different traffic categories can have different module types, implementing different behavior.

   so

   - tsnswitch has ingresstrafficfiltering
   - tsndevice has egress and ingress traffic filtering
   - this adds a streamfilteringlayer to the network node
   - the streamfilteringlayer has optional ingress and egress filtering submodules
   - the proper ones are enabled according to the parameters
   - by default the ingress/egress filtering submodule is a SimpleIeee8021qFilter,
   that can do per-stream filtering. it has a configurable number of traffic streams,
   and a meter, a filter and a gate for each. additionally, there is the default route
   which unfiltered packets take by default.
   - the meter can do metering
   - the filter drops the packets (might be based on the metering)
   - the gate is an interactive gate by default, so its always open
   - example

   - the classifier is a streamclassifier by default -> classify according to named streams
   - token bucket filtering can be done with the SingleRateTwoColorMeter module
   - this is a queueing element that has one token bucket
   - can specify a committed information rate and a committed burst rate
   - the rate of token regeneration defines the information rate
   - the max number of tokens the burst size (if lots of tokens accumulate, the node can transmit faster/until they are depleted)
   - the meter actually labels packets green or red, according to the number of tokens available (if there are no tokens, then red)
   - the filter is a labelfilter and it drops red packets
   - note check out other meter modules (DualRateThreeColorMeter)
   - the submodules in the simpleieee8021qfilter doesn't have to be the same type for the different traffic categories (e.g. check out the mixing shapers)

Statistical Policing Overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this showcase, we'll use the :ned:`SlidingWindowRateMeter` and the :ned:`StatisticalRateLimiter` modules for basic statistical filtering.
The :ned:`SlidingWindowRateMeter` module measures datarate and packetrate by summing up the
packet bytes over the specified time window, and attaches a ``rateTag`` to each packet.
The :ned:`StatisticalRateLimiter` is a filter module. We can specify a maximum data rate with the :par:`maxDataRate` parameter. The module
drops packets that would exceed this limit.

.. - how we do statistical filtering
   - the slidingwindowratemeter can do that. and the StatisticalRateDropper
   - the slidingwindowratemeter measures the datarate and packetrate of packets passing through in configurable time window (10ms by default).
   it attaches datarate and packetrate tags to packets.
   - the StatisticalRateDropper have a maxDataRate parameter and it drops excessive packets based on the attached tag

This is a simple statistical filtering, because the data rate is measured with a sliding window. the precision depends on the timewindow size
-> like how fast the filter reacts to changes in incoming data rate? and how precisely the outgoing datarate sticks to the configured maximum. TODO

The Network
~~~~~~~~~~~

.. Here is the network:

The simulation uses the TsnLinear network, with two TsnDevice modules connected via a TsnSwitch:

.. figure:: media/Network.png
   :align: center
   :width: 100%

The Configuration
~~~~~~~~~~~~~~~~~

   In this configuration we use a sliding window rate meter in combination with a
   statistical rate limiter. The former measures the thruput by summing up the
   packet bytes over the time window, the latter drops packets in a probabilistic **(?)**
   way by comparing the measured datarate to the maximum allowed datarate.

.. Here is the configuration:

   .. literalinclude:: ../omnetpp.ini
      :language: ini

In this simulation, the client generates two traffic streams with a sinusoidally changing data rates.
The filtering layer in the switch limits this traffic to a nominal rate by dropping excessive packets.

Traffic
+++++++

The source is configured to generate two traffic streams (we classify them to the best effort and video traffic classes in the next section, based on source port).
The best effort and video data streams have a sinusoidally changing data rate with a mean of 40 and 20 Mbps, respectively:

.. literalinclude:: ../omnetpp.ini
   :language: ini
   :start-at: client
   :end-before: enable outgoing streams

Stream Identification, Encoding and Decoding
++++++++++++++++++++++++++++++++++++++++++++

We enable outgoing streams in the client (this adds a stream indentifyer layer), and assign packets to the best effort and video named streams based on
source port. The stream coder encodes the streams with PCP numbers. We configure the switch simiarly to decode the named stream based on PCP. Here is the relevant
configuration:

.. literalinclude:: ../omnetpp.ini
   :language: ini
   :start-at: enable outgoing streams
   :end-before: enable per-stream filtering

Per-Stream Filtering
++++++++++++++++++++

We enable ingress per-stream filtering in the switch. As mentioned before, this setting adds a StreamFilteringLayer to the bridging layer of the switch,
with a :ned:`SimpleIeee8021qFilter` module as the ingress filter. We set the number of traffic streams to 2; the default path is enabled by default, though
it won't be used in this scenario, as all packets are part of either the best effort or video stream. We configure the priority of the streams in the classifier **(?)**.
We set the type of both meter submodules type to :ned:`SlidingWindowRateMeter`, and configure the time window to be 10ms. We specify the maximum
data rates. The data rates are sinusoids with a mean of 40 and 20 Mbps, thus they fluctuate around mean. The filter limits the data rate to the mean value.


.. literalinclude:: ../omnetpp.ini
   :language: ini
   :start-at: enable per-stream filtering

Results
-------

.. Here are the results:

Let's examine the results. The following chart shows the data rates for the two traffic streams in the source:

.. figure:: media/ClientApplicationTraffic.png
   :align: center

The following charts show the incoming, outgoing (shaped), and dropped data rates, for the two traffic categories:

.. figure:: media/datarate_be.png
   :align: center

.. figure:: media/datarate_vi.png
   :align: center

The filter limits the data rate to the nominal value. Note that the shaped data rate is less than the incoming rougly with the amount of dropped data rate.

**TODO** this might not be needed cos its the same

.. figure:: media/source_sink.png
   :align: center

Sources: :download:`omnetpp.ini <../omnetpp.ini>`

Discussion
----------

Use `this <https://github.com/inet-framework/inet/discussions/794>`__ page in the GitHub issue tracker for commenting on this showcase.

