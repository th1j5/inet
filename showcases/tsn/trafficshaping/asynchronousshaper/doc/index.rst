Asynchronous Shaping
====================

Goals
-----

The asynchronous traffic shaper can smooth traffic while allowing some burstiness.
This reduces delay and increases link utilization compared to the credit-based shaper.
Also, it doesn't require synchronized time in network nodes.

In this example we demonstrate how to use the asynchronous traffic shaper.

**TODO** according to the docs, the CBS requires coordinated time; why?

| INET version: ``4.4``
| Source files location: `inet/showcases/tsn/trafficshaping/asynchronousshaper <https://github.com/inet-framework/tree/master/showcases/tsn/trafficshaping/asynchronousshaper>`__

The Model
---------

Overview
~~~~~~~~

Conceptually, the asynchronous traffic shaper meters the data rate of incoming traffic, and calculates an eligibility time for all packets,
i.e. when the packet should be sent. Sending packets at their respective eligibility time results in the shaped output traffic.
As traffic streams are reshaped per-hop, no synchornized time is necessary among network nodes, and channel utilization can be optimized
at each link.

.. **V1** As traffic streams are reshaped per-hop, comforming to the specified requirements, such as delay, no synchronized time is necessary.

The eligibility time is calculated by the asyncronous shaper algorithm.
The shaper has two parameters that can be specified (as opposed to the one parameter of the credit-based shaper), 
the `committed information rate`, and the `committed burst rate`.
The committed information rate is similar to the idle slope parameter of the credit-based shaper in that it specifies and average outgoing
data rate that the traffic is limited to. The committed burst rate specifies that allowed burst size.

**TODO** It uses a token bucket to do that.

In INET, the asynchronous shaper mechanism is implemented by four modules:

- :ned:`EligibilityTimeMeter`: calculates eligibility time (in bridging layer)
- :ned:`EligibilityTimeFilter`: filters expired packets (?)(in bridging layer)
- :ned:`EligibilityTimeQueue`: stores packets ordered by eligibility time (in interface)
- :ned:`EligibilityTimeGate`: pushes packets at the eligibility time for transmission (in interface)

.. **how to insert**

Each of these modules have their place in the TSN node architecture. 
To enable asynchronous traffic shaping in a TSN switch, for example, we need to do the following: 

- The :ned:`EligibilityTimeMeter` and :ned:`EligibilityTimeFilter` modules are in the bridging layer of the switch. First, we enable ingress
  filtering in the switch:

  .. code-block:: ini

     *.switch.hasIngressTrafficFiltering = true

  This adds a :ned:`StreamFilterLayer` to the bridging layer. By default, this module contains a :ned:`SimpleIeee8021qFilter` module,
  which has submodules to implement per-stream filtering and policing. **TODO** the details (like module screenshot) should be in the filtering and policing showcases?

  We set the type of the meter and filter submodules: 

  .. code-block:: ini

     *.switch.bridging.streamFilter.ingress.meter[*].typename = "EligibilityTimeMeter"
     *.switch.bridging.streamFilter.ingress.filter[*].typename = "EligibilityTimeFilter"

- The :ned:`EligibilityTimeQueue` and :ned:`EligibilityTimeGate` is in the MAC layer of network interfaces. We enable egress traffic shaping in the switch:

  .. code-block:: ini

     ``*.switch.hasEgressTrafficShaping = true``

  This adds a :ned:`Ieee8021qTimeAwareShaper` to the MAC layer of all interfaces. Similarly to the credit-based shaper, it is convenient to insert the
  asynchronous shaper modules into the time-aware shaper module, as it has all the necessary submodules (e.g. queues) and a configurable number of traffic classes.
  We just need to override some of the submodule types, namely the queue and the transmission selection algorithm:

  .. code-block:: ini

     *.switch.eth[*].macLayer.queue.queue[*].typename = "EligibilityTimeQueue"
     *.switch.eth[*].macLayer.queue.transmissionSelectionAlgorithm[*].typename = "Ieee8021qAsynchronousShaper"

  .. note:: The :ned:`Ieee8021qAsynchronousShaper` module extends :ned:`EligibilityTimeGate` without setting any parameters, so the two modules are the same.



.. The asynchronous shaper architecture outlined above is added to briding layer and interface MAC layer in the switch.

.. To enable asynchronous traffic shaping in a TSN switch, for example, we need to do the following: 

   - Enable ingress filtering in the switch: ``*.switch.enableIngressTrafficFiltering = true``; this adds a :ned:`StreamFilterLayer` to the bridging layer. By default, this module contains a :ned:`SimpleIeee8021qFilter` module,
   which can implement per-stream filtering and policing:

   TODO

   - Set the type of the `meter` submodules (one per stream) in the stream filtering layer: ``*.switch.bridging.streamFilter.ingress.meter[*].typename = "EligibilityTimeMeter"``

   - Set the :par:`committedInformationRate` and :par:`committedBurstSize` parameters of the meter submodules (one per stream):

   .. code-block::

      *.switch.bridging.streamFilter.ingress.meter[0].committedInformationRate = 20Mbps
      *.switch.bridging.streamFilter.ingress.meter[0].committedBurstSize = 10000B

   - Enable egress traffic shaping in the switch: ``*.switch.hasEgressTrafficShaping = true`` (this adds :ned:`Ieee8021qTimeAwareShaper` submodules the interfaces) 

   - Set the queue type to async queue
   - Set the gate type to async shaper

.. **parameters**

- To configure the asynchronous traffic shaping, we can specify the number of traffic classes in the time-aware shaper modules, and set the following parameters
  of the asynchronous shaper:

  - :par:`committedInformationRate` and :par:`committedBurstSize` parameters in :ned:`EligibilityTimeMeter`. These specify the nominal outgoing data rate and the allowed burst size of the shaper
  - :par:`maxResidenceTime` parameter in :ned:`EligibilityTimeMeter`. Packets are dropped by the :ned:`EligibilityTimeFilter` if the simulation time equals ``eligibility time + max residence time``.

.. note:: For other parameters of the modules making up the asynchronous shaper, check the NED documentation of the modules.

The Configuration
~~~~~~~~~~~~~~~~~

Network
+++++++

.. There are three network nodes in the network. The client and the server are
   :ned:`TsnDevice` modules, and the switch is a :ned:`TsnSwitch` module. The
   links between them use 100 Mbps :ned:`EthernetLink` channels.

The network contains three network nodes. The client and the server (:ned:`TsnDevice`) are
connected through the switch (:ned:`TsnSwitch`), with 100Mbps :ned:`EthernetLink` channels: 

.. figure:: media/Network.png
   :align: center

Overview
++++++++

Similarly to the credit-based shaper showcase, we configure the client to generate two streams of fluctuating traffic, and to assign them to two different traffic categories.
We add asynchronous traffic shapers to the switch that smooths outgoing traffic for each traffic category independently.

Traffic
+++++++

Similarly to the Time-Aware Shaping and Credit-based Shaping showcases, we want to observe only the effect of the credit-based shaper on the traffic. 
Thus our goal is for the traffic to only get altered in the traffic shaper, and avoid any unintended traffic shaping effect in other parts of the network.

.. There are four applications in the network creating two independent data streams
   between the client and the server. The data rate of both streams are ~48 Mbps at
   the application level in the client.

Traffic configuration is similar to the Credit-Based Shaper showcase, with slightly different data rates. 
We configure two traffic source applications in the client, creating two independent data streams between the client and the server. 
The data rate of the streams fluctuates randomly around 40 and 20 Mbps mean values, respectively, but the links in the network are not saturated. 
Later on, we configure the traffic shaper to limit the data rate to the nominal values of 40 and 20 Mbps for the data streams. Here is the traffic 
configuration:

.. literalinclude:: ../omnetpp.ini
   :start-at: client applications
   :end-before: outgoing streams
   :language: ini

Stream Identification and Encoding
++++++++++++++++++++++++++++++++++

We classify packets to the two traffic categories (best effort and video) the same way as in the credit-based shaper showcase. To summarize:

- We enable IEEE 802.1 stream identification and stream encoding in the client. 
- We configure the stream identifier module in the bridging layer to assign outgoing packets to named streams by UDP destination port. 
- The stream encoder sets the PCP number according to the assigned stream name.
- After transmission, the switch decodes the streams by the PCP number.

.. Here is the relevant configuration:

.. **TODO** openable

.. note::

   .. raw:: html

      <details>
      <summary>The relevant configuration (click to open/close)</summary>

   .. literalinclude:: ../omnetpp.ini
      :start-at: outgoing streams
      :end-before: ingress per-stream filtering
      :language: ini

   .. raw:: html

      </details>

.. The two streams have two different traffic classes: best effort and video. The
   bridging layer identifies the outgoing packets by their UDP destination port.
   The client encodes and the switch decodes the streams using the IEEE 802.1Q PCP
   field.

   .. literalinclude:: ../omnetpp.ini
      :start-at: outgoing streams
      :end-before: ingress per-stream filtering
      :language: ini

.. The asynchronous traffic shaper requires the transmission eligibility time for
   each packet to be already calculated by the ingress per-stream filtering.

Asynchronous Traffic Shaping
++++++++++++++++++++++++++++

Per-Stream Filtering
####################

Next, we configure the asynchronous shaper modules in the interfaces, the eligibility-time meter and filter.
We enable ingress per-stream traffic filtering in the switch (as described above, this adds a :ned:`StreamFilteringLayer` to the switch; the stream filtering
layer has an ingress filter (:ned:`SimpleIeee8021qFilter`) submodule that we configure to contain the eligibility-time meters and filters). We configure two traffic streams
in the ingress filter. The ingress filter has a :ned:`StreamClassifier` module at the beginning (where packets go in); we specify that best effort
packets should use gate 0, and video packets gate 1. Then we override the type of the ``meter`` submodules.

Next, we need to add the asynchronous shaper modules (:ned:`EligibilityTimeMeter` and :ned:`EligibilityTimeFilter`) to the bridging layer of the switch,
and configure them:

- We enable ingress per-stream traffic filtering in the switch (as described above, this adds a :ned:`StreamFilteringLayer` to the switch; the stream filtering
  layer has an ingress filter (:ned:`SimpleIeee8021qFilter`) submodule that we configure to contain the eligibility-time meters and filters).
- As we want per-stream filtering, we configure two traffic streams
  in the ingress filter./for our two streams.
- We need to configure the mapping in the classifier (:ned:`StreamClassifier`) in the ingress filter. This tells the classifier to send ``best effort`` streams
  to gate 0, and video streams to gate 1. 
- We override the type of the ``meter`` submodules with :ned:`EligibilityTimeMeter`, and configure the committed information rate and committed burst size
  parameters. Also, we set a max residence time of 10ms in the meter; this ensures that packets waiting more that 10ms in the switch are dropped by the filter
  submodule that we configure next.
- We override  the type of the ``meter`` submodules with :ned:`EligibilityTimeFilter`.

Here is the configuration doing the above:

.. literalinclude:: ../omnetpp.ini
   :start-at: ingress per-stream filtering
   :end-before: egress traffic shaping
   :language: ini

Thus far, we have two traffic categories encoded with PCP numbers and named streams, the meter calculates the eligibility time as per the parameters,
and the filter drops any timed-out packets. Now we just need to add the necessary queues and gates.

Egress Traffic Shaping
######################

.. Then we set the parameters.

.. Then we override the filter submodules.

.. .. literalinclude:: ../omnetpp.ini
   :start-at: ingress per-stream filtering
   :end-before: egress traffic shaping
   :language: ini

The traffic shaping takes place in the outgoing network interface of the switch
where both streams pass through. The traffic shaper limits the data rate of the
best effort stream to 40 Mbps and the data rate of the video stream to 20 Mbps.
The excess traffic is stored in the MAC layer subqueues of the corresponding
traffic class.

- enable egress traffic shaping
- two traffic classes
- queue and transmission selection algorithms

.. literalinclude:: ../omnetpp.ini
   :start-at: egress traffic shaping
   :language: ini

Results
-------

Let's examine how the traffic data rate changes in the network, from the client to the server.
The first chart displays the client application and the shaper incoming traffic data rate, per-stream:

.. figure:: media/client_shaper.png
   :align: center

The data rate of the client varies randomly over time for both traffic classes
but the averages are the nominal values of ~42 and 21 Mbps.
For each stream, the traffic in the client and the shaper incoming traffic is similar. The data rate is
higher in the shaper because it already includes protocol overhead, such as the Ethernet header.

The next chart compares the incoming and outgoing traffic in the shaper, so we can observe
how the traffic is shaped. The nominal data rate is displayed with the two dashdotted lines:

.. figure:: media/shaper_both.png
   :align: center

The shaper allows some burst traffic (more than the nominal rate), but in general smooths traffic
according to the committed data rate parameter.

The next chart displays the shaper outgoing and the server application traffic data rate:

.. figure:: media/shaper_server.png
   :align: center

The traffic doesn't change significantly in this part of the network. Again, the shaper data rate is
slighly higher due to protocol overhead.

Thus, as per our goal, traffic shaping happens only in the traffic shaper in the network.

   the randomly varying data rate of the incoming traffic is already transformed
   here into a quite stable data rate

.. The first diagram shows the data rate of the application level outgoing traffic
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

.. The next diagram shows the data rate of the already shaped outgoing traffic of
   the traffic shapers. This data rate is still measured inside the outgoing network
   interface of the switch but at a different location. As it is quite apparent,
   the randomly varying data rate of the incoming traffic is already transformed
   here into a quite stable data rate.

.. .. figure:: media/TrafficShaperOutgoingTraffic.png
   :align: center

.. **TODO** seqchart in the beginning when there is no shaping; another at 0.1s

Let's examine the traffic on sequence charts. The following chart shows the beginning of the
simulation, when the client application traffic is less than the committed data rate of the
shaper, thus no traffic shaping takes place. In the switch, incoming packets are forwarded
as soon as they are received (best effort packets are blue, video packets are red):

.. figure:: media/seqchart_noshaping.png
   :align: center

The next sequence chart shows a later part of the simulation, when the data rate of the client
is higher than the committed data rate of the shaper, and traffic is smoothed, but some bursting is allowed:

.. figure:: media/seqchart_shaping.png
   :align: center

The next diagram shows the queue lengths of the traffic classes in the outgoing
network interface of the switch. The queue lengths increase over time because
the data rate of the incoming traffic of the traffic shapers is greater than
the data rate of the outgoing traffic on average, and packets are not dropped:

.. figure:: media/TrafficShaperQueueLengths.png
   :align: center

The next diagram shows the number of tokens in the shaper for both traffic categories,
and the shaper outgoing data rate on two charts with the same x-axis:

.. figure:: media/tokens.png
   :align: center

We can observe the (inverse) relationship between the number of tokens and the data rate. When the token count is high,
the shaper is allowed to send burts for some time.

TODO

.. figure:: media/RemainingEligibilityTime.png
   :align: center

.. figure:: media/et.png
   :align: center

TODO

.. figure:: media/TransmittingStateAndGateStates_.png
   :align: center

.. figure:: media/TransmittingStateAndGateStates.png
   :align: center

The next diagram shows the relationships (for both traffic classes) between
the gate state of the transmission gates and the transmitting state of the
outgoing network interface.

.. figure:: media/TrafficShaping.png
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

Use `this <https://github.com/inet-framework/inet/discussions/799>`__ page in the GitHub issue tracker for commenting on this showcase.

