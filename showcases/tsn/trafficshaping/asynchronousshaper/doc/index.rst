Asynchronous Shaping
====================

Goals
-----

The asynchronous traffic shaper can smooth traffic while allowing some burstiness.
This reduces delay and increases link utilization compared to the credit-based shaper.
Also, it doesn't require synchronized time in network nodes.

In this example we demonstrate how to use the asynchronous traffic shaper.

| INET version: ``4.4``
| Source files location: `inet/showcases/tsn/trafficshaping/asynchronousshaper <https://github.com/inet-framework/tree/master/showcases/tsn/trafficshaping/asynchronousshaper>`__

The Model
---------

Overview
~~~~~~~~

The asynchronous traffic shaper 

Conceptually, the async shaper meters the data rate of incoming traffic, calculates an eligibility time for all packets,
i.e. when the packet should be sent. Sending packets at their respetive eligibility time results in the shaped output traffic.
**V1** As traffic streams are reshaped per-hop, comforming to the specified requirements, such as delay, no synchronized time is necessary.
**V2** As traffic streams are reshaped per-hop, no synchornized time is necessary among network nodes, and channel utilization can be optimized
at each link.

The eligibility time is calculated by the asyncronous shaper algorithm. It uses a token bucket to do that.
The shaper has two parameters that can be specified (as opposed to the one parameter of the credit-based shaper), the `committed information rate`, and the `committed burst rate`.
The committed information rate is similar to the idle slope parameter of the credit-based shaper in that it specifies and average outgoing
data rate that the traffic is limited to. The committed burst rate specifies that allowed burst size.

In INET, the asynchronous shaper mechanism is implemented by four modules:

- eligibilitytimemeter: calculates eligibility time (in bridging layer)
- eligibilitytimefilter: filters expired packets (?)(in bridging layer)
- eligibilitytimequeue: stores packets ordered by eligibility time (in interface)
- eligibilitytimegate: pushes packets at the eligibility time for transmission (in interface)

**how to insert**

**parameters**

The Configuration
~~~~~~~~~~~~~~~~~

Network
+++++++

There are three network nodes in the network. The client and the server are
:ned:`TsnDevice` modules, and the switch is a :ned:`TsnSwitch` module. The
links between them use 100 Mbps :ned:`EthernetLink` channels.

.. figure:: media/Network.png
   :align: center

There are four applications in the network creating two independent data streams
between the client and the server. The data rate of both streams are ~48 Mbps at
the application level in the client.

.. literalinclude:: ../omnetpp.ini
   :start-at: client applications
   :end-before: outgoing streams
   :language: ini

The two streams have two different traffic classes: best effort and video. The
bridging layer identifies the outgoing packets by their UDP destination port.
The client encodes and the switch decodes the streams using the IEEE 802.1Q PCP
field.

.. literalinclude:: ../omnetpp.ini
   :start-at: outgoing streams
   :end-before: ingress per-stream filtering
   :language: ini

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
in the client. The data rate varies randomly over time for both traffic classes
but the averages are the same.

.. figure:: media/ClientApplicationTraffic.png
   :align: center

The next diagram shows the data rate of the incoming traffic of the traffic
shapers. This data rate is measured inside the outgoing network interface of
the switch. This diagram is somewhat different from the previous one because
the traffic is already in the switch, and also because it is measured at a
different protocol level.

.. figure:: media/TrafficShaperIncomingTraffic.png
   :align: center

The next diagram shows the data rate of the already shaped outgoing traffic of
the traffic shapers. This data rate is still measured inside the outgoing network
interface of the switch but at a different location. As it is quite apparent,
the randomly varying data rate of the incoming traffic is already transformed
here into a quite stable data rate.

.. figure:: media/TrafficShaperOutgoingTraffic.png
   :align: center

**TODO** seqchart in the beginning when there is no shaping; another at 0.1s

The next diagram shows the queue lengths of the traffic classes in the outgoing
network interface of the switch. The queue lengths increase over time because
the data rate of the incoming traffic of the traffic shapers is greater than
the data rate of the outgoing traffic, and packets are not dropped.

.. figure:: media/TrafficShaperQueueLengths.png
   :align: center

TODO

.. figure:: media/NumberOfTokens.png
   :align: center

TODO

.. figure:: media/RemainingEligibilityTime.png
   :align: center

TODO

.. figure:: media/TransmittingStateAndGateStates.png
   :align: center

The next diagram shows the relationships (for both traffic classes) between
the gate state of the transmission gates and the transmitting state of the
outgoing network interface.

.. figure:: media/TrafficShaping.png
   :align: center

The last diagram shows the data rate of the application level incoming traffic
in the server. The data rate is somewhat lower than the data rate of the
outgoing traffic of the corresponding traffic shaper. The reason is that they
are measured at different protocol layers.

.. figure:: media/ServerApplicationTraffic.png
   :align: center

Sources: :download:`omnetpp.ini <../omnetpp.ini>`

Discussion
----------

Use `this <https://github.com/inet-framework/inet/discussions/799>`__ page in the GitHub issue tracker for commenting on this showcase.

