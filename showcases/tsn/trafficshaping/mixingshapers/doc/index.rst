Mixing Different Shapers
========================

Goals
-----

.. In this example we demonstrate how to use different traffic shapers in the same
   network interface.

The different traffic shapers in INET can be combined arbitrarily in any network node.
In this showcase, we demonstrate using a Credit-Based Shaper (CBS) and an Asynchronous Traffic Shaper
for shaping two different traffic streams in a TSN switch.

| INET version: ``4.4``
| Source files location: `inet/showcases/tsn/trafficshaping/mixingshapers <https://github.com/inet-framework/inet/tree/master/showcases/tsn/trafficshaping/mixingshapers>`__

The Model
---------

.. The whole thing is modular so you can basically insert and use any traffic shaper and combine them.
   For example, as mentioned/describen in the other showcases, the time-aware shaper can be used as a module
   to insert other shapers into (CBS, ATS), and, when the TAS is enabled, it can provide time aware shaping as well.

.. Now we insert a CBS and an ATS into a switch. We use the time aware shaper module to insert these into, but
   dont enable time aware shaping. We use two traffic streams, each shaped by one of the shapers.

.. We then observe how the two traffic streams are shaped in the `Results` section.

INET's traffic shaping infrastructure is modular, and the different traffic shapers can be arbitrarily combined.
It is possible to use different or multiple traffic shapers per-stream, in a network node. For example, as described in the previous
traffic shaping showcases, the time-aware shaper module can be used
to insert other shapers into (CBS, ATS). The time-aware shaper can optionally provide time aware shaping as well.

In this showcase, we insert a credit-based shaper and an asynchronous traffic shaper into a time-aware shaper.
We use two traffic streams; one of them is shaped with the CBS, the other with the ATS. 
Time-aware shaping isn't enabled.


.. We use the credit-based shaper to shape one traffic stream, and the ATS to shape another.

.. We don't use time-aware shaping. 

Overview
~~~~~~~~

The following is a quick overview of the traffic shaper architecture in INET.
For more details, check the NED documentation of traffic shaper modules and the other shaper showcases.

.. - the time aware shaper is a base for all these
   - the cbs is a packetgate module
   - the ats is more complex, as it is made up of 4 modules:
   TODO

- The :ned:`Ieee8021qTimeAwareShaper` module is added to interfaces of TSN network nodes when the :par:`hasEgressTrafficShaping` parameter is set to ``true``.
  This module has a configurable number of traffic classes (:par:`numTrafficClasses`), and a ``queue`` and ``transmissionSelectionAlgorithm`` submodule for each
  traffic class. A convenient way to add CBS or ATS capability to an interface is to override the type of these submodules
  with :ned:`Ieee8021qCreditBasedShaper` or :ned:`Ieee8021qAsynchronousShaper`.
- :ned:`Ieee8021qCreditBasedShaper` and :ned:`Ieee8021qAsynchronousShaper` are packet gate modules, and take the place of the ``transmissionSelectionAlgorithm`` module in the TAS.
- ATS also needs a :ned:`EligibilityTimeQueue` as the queue in the TAS, and an :ned:`EligibilityTimeMeter` and :ned:`EligibilityTimeFilter` in the bridging layer
  of the TSN network node.

.. The type of these submodules can be overriden with :ned:`Ieee8021qCreditBasedShaper` or :ned:`Ieee8021qAsynchronousShaper` to add CBS or ATS
  to the network node/interface. 

.. This module has `queue` and `transmissionSelectionAlgorithm` submodules

**TODO** Not even sure this is needed. Just the second line.

Configuration
~~~~~~~~~~~~~

The Network
+++++++++++

There are three network nodes in the network. The client and the server are
:ned:`TsnDevice` modules, and the switch is a :ned:`TsnSwitch` module. The
links between them use 100 Mbps :ned:`EthernetLink` channels.

.. figure:: media/Network.png
   :align: center

Traffic
+++++++

Similarly to the other traffic shaping showcases, we want to observe how the traffic shapers change
the traffic. When generating traffic, we make sure that the traffic is only changed significantly
in the shapers (i.e. other parts of the network have no traffic shaping effect).

.. There are four applications in the network creating two independent data streams
   between the client and the server. The data rate of both streams are ~48 Mbps at
   the application level in the client.

We create two sinusoidally changing traffic streams (called ``best effort`` and ``video``) in the ``client``, with an average data rate of 40 and 20 Mbps.
The two streams are terminated in two traffic sinks in the ``server``:

.. literalinclude:: ../omnetpp.ini
   :start-at: client applications
   :end-before: outgoing streams
   :language: ini

Stream Identification and Encoding
++++++++++++++++++++++++++++++++++

The two streams have two different traffic classes: best effort and video. The
bridging layer identifies the outgoing packets by their UDP destination port.
The client encodes and the switch decodes the streams using the IEEE 802.1Q PCP
field.

.. literalinclude:: ../omnetpp.ini
   :start-at: outgoing streams
   :end-before: ingress per-stream filtering
   :language: ini

Traffic Shaping
+++++++++++++++

The asynchronous traffic shaper requires the transmission eligibility time for
each packet to be already calculated by the ingress per-stream filtering.

.. literalinclude:: ../omnetpp.ini
   :start-at: ingress per-stream filtering
   :end-before: egress traffic shaping
   :language: ini

The traffic shaping takes place in the outgoing network interface of the switch
where both streams pass through. The traffic shaper limits the data rate of the
best effort stream to 40 Mbps and the data rate of the video stream to 20 Mbps.
The excess traffic is stored in the MAC layer subqueues of the corresponding
traffic class.

.. literalinclude:: ../omnetpp.ini
   :start-at: egress traffic shaping
   :language: ini

Results
-------

The first diagram shows the data rate of the application level outgoing traffic
in the client. The data rate varies randomly over time but the averages are the
same.

The following chart shows the client application and traffic shaper incoming data rate:

.. figure:: media/client_shaper.png
   :align: center

As per our goal, the traffic doesn't change significantly before getting to the shaper.
The data rate in the shaper is a bit higher due to protocol overhead, but the traffic
measured in the two locations follow the same shape.

The next diagram shows the data rate of the incoming traffic of the traffic
shapers of the outgoing network interface in the switch. This is different
from the previous because the traffic is already in the switch and it is also
measured at different protocol level.

The next chart shows the incoming and outgoing traffic in the shaper, so we can observe
the shaper's effect on the traffic:

.. figure:: media/shaper_both.png
   :align: center

The best effort traffic is shaped by the CBS. The shaper limits the data rate to the nominal value of 40Mbps,
without bursts. The video stream is shaped by the ATS, and in addition to limiting the data rate to the nominal value,
some bursts are allowed whenever the traffic increases above the nominal rate.

The next diagram shows the data rate of the already shaped outgoing traffic of
the outgoing network interface in the switch. The randomly varying data rate of
the incoming traffic is transformed into a quite stable data rate for the outgoing
traffic.

The next chart shows the shaper outgoing and server application traffic:

.. figure:: media/shaper_server.png
   :align: center

We can observe that the traffic doesn't change significantly here, thus it only changes in the shaper.

TODO

.. figure:: media/TransmittingStateAndGateStates.png
   :align: center

The next diagram shows the queue lengths of the traffic shapers in the outgoing
network interface of the switch. The queue lengths increase over time because
the data rate of the incoming traffic of the shapers is greater than the data
rate of the outgoing traffic.

.. figure:: media/TrafficShaperQueueLengths.png
   :align: center

The next diagram shows the relationships between the number of credits, the gate
state of the credit based transmission selection algorithm, and the transmitting
state of the outgoing network interface for the best effort traffic class.

.. figure:: media/BestEffortTrafficClass.png
   :align: center

The next diagram shows the relationships between the number of credits, the gate
state of the credit based transmission selection algorithm, and the transmitting
state of the outgoing network interface for the video traffic class.

.. figure:: media/VideoTrafficClass.png
   :align: center

.. The last diagram shows the data rate of the application level incoming traffic
   in the server. The data rate is somewhat lower than the data rate of the
   outgoing traffic of the corresponding traffic shaper. The reason is that they
   are measured at different protocol layers.

   .. figure:: media/ServerApplicationTraffic.png
      :align: center

Sources: :download:`omnetpp.ini <../omnetpp.ini>`

Discussion
----------

Use `this <https://github.com/inet-framework/inet/discussions/801>`__ page in the GitHub issue tracker for commenting on this showcase.

