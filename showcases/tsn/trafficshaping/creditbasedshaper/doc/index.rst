Credit-Based Shaping
====================

Goals
-----

The credit-based traffic shaper can be used to smooth-out traffic and reduce bursting in network nodes' outgoing interfaces. This adds gaps between successive packets of an incoming packet burst, 
and can enable different priority traffic to get transmitted in the gaps, and thus reduce its delay.

In this example we demonstrate how to use the credit-based traffic shaper.

.. **TODO** some interesting stuff to show? -> shaping in general increases delay even for high priority frames. but can overall decrease delay (as it decreases delay for lower priority frames)

| INET version: ``4.4``
| Source files location: `inet/showcases/tsn/trafficshaping/creditbasedshaper <https://github.com/inet-framework/inet/tree/master/showcases/tsn/trafficshaping/creditbasedshaper>`__

The Model
---------

Overview
~~~~~~~~

..   - the credit based shaper can be added to network interfaces as part of a Time aware shaper module
   - the time aware shaper module is a queue in the network interface
   - the credit based shaper can be added as the optional transmission selection algorithm submodule of the time aware shaper
   - the credit based shaper has an IPacketGate interface/acts a packet gate module (which does what? sometimes dont let packet through?)
   - by default, the gates of the time aware shaper are always open, so there is no time aware shaping
   - thus its a credit based shaper that can optionally be combined with time aware shaping

   so

   - the Ieee8021qCreditBasedShaper is a packet gate module, which can be combined with a packet queue to implement the credit based shaper algorithm
   - a convenient way to add packet queues to the credit based shaper is to insert it into a time aware shaper module
   - the time aware shaper module is a compound packet queue, that can be inserted into network interfaces
   - it has an optional transmission selection algorithm which can be a credit based shaper
   - by default, the time aware shaper's gates are always open, thus there is no time aware shaping
   - so by inserting a credit based shaper into a time aware shaper, the time aware shaping can optionally be added
   - this for each traffic class

   structure

   - what does the credit based shaper do
   - parameters
   - inserting

The shaper maintains 
an amount of credit that changes depending on whether the interface is currently transmitting or idle. 
Frame transmission is only allowed when the credit count is non-negative. When frames are transmitted, credit decreases with the channel data rate (`send slope`) 
until the  transmission is complete. When there is no transmission, credit increases with a rate called `idle slope`. 
The next frame is transmitted when the credit is zero or positive. The idle slope controls the average outgoing data rate.

   , and can be specified with the :par:`idleSlope` parameter (in bps) of the shaper.

.. The idle and send slopes can be specified with parameters.

In INET, the credit-based shaper is implemented by the :ned:`Ieee8021qCreditBasedShaper` simple module. This is a packet gate module 
that can be combined with a packet queue to implement the credit based shaper algorithm. A convenient way to combine a credit-based shaper with queues and
to insert it into network interfaces is to use a :ned:`Ieee8021qTimeAwareShaper`. This module already has queue submodules, supports a configurable number
of traffic classes, and fits into Ethernet interfaces by replacing the default packet queue module in the MAC layer of the interface.
The credit-based shaper module takes the place of the optional ``transmissionSelectionAlgorithm`` submodule of the time-aware shaper:

.. **TODO** transmissionSelection decides the priority -> reverse order

.. figure:: media/timeawareshaper.png
   :align: center

Packets arriving in the above module are classified to the different traffic categories based on their PCP number. The priority is decided by the ``transmissionSelection`` submodule.
This is a :ned:`PriorityScheduler` configured to work in reverse order, i.e., priority increases with traffic class index (on the image above, ``video`` has priority over ``best effort``).

.. note:: By default, the time-aware shaper doesn't do any time-aware shaping, as its gates are always open. Thus its possible to use the combined time-aware
   shaper/credit-based shaper module as only a credit-based shaper this way, or add optional time-aware shaping as well (by specifying gate schedules).

..  in addition to credit-based shaping

.. The shaper has parameters for credit decrease and accumulation rate (:par:`idleSlope` and :par:`sendSlope`, both in Mbps),
   and max interference size, i.e. how much overlap can there be with the next frame???? **TODO**

.. **TODO** max interference size?

**TODO** other parameters, check the NED doc

.. initial, min and max credit count. The threshold where transmission is possible can also be specified (:par:`transmitCreditLimit`).

**TODO** the number of traffic classes can be specified with the parameter of the time aware shaper -> check out the showcase -> actually earlier

**TODO** CBS details

**TODO** channel data rate is the parameter in idleslope; not the same as the shaper data rate

.. **TODO** is it possible to have just one cbs and not per-traffic-class? -> numTrafficClasses=1?

The Configuration
~~~~~~~~~~~~~~~~~

The Network
+++++++++++

The showcase uses the following network:

.. figure:: media/Network.png
   :align: center

There are three network nodes in the network. The client and the server are
:ned:`TsnDevice` modules, and the switch is a :ned:`TsnSwitch` module. The
links between them are 100 Mbps :ned:`EthernetLink` channels.

Overview
++++++++

In this simulation, we configure the client to generate two streams of fluctuating traffic, and to assign them to two traffic categories.
We insert credit-based shapers for each category into the switch's outgoing interface (eth1) to smooth traffic.

Traffic
+++++++

.. Similarly as in the TODO showcase, we want to configure the traffic in such as way that it is only altered in the traffic shaper.

Similarly to the ``Time-Aware Shaping`` showcase, we want to observe only the effect of the credit-based shaper on the traffic. Thus our goal is
for the traffic to only get altered in the traffic shaper, and minimize unintended traffic shaping effect in other parts of the network.

.. so overview

   - we configure two traffic sources in the client
   - the data rate fluctuates around 42 and 21 Mbps mean values
   - that means that sometimes the data rate is more than the nominal/mean values
   - we configure the traffic shaper to limit the data rate to the nominal/mean values of 42 and 21 Mbps for the data streams

We configure two traffic source applications in the client, creating two independent data streams
between the client and the server. The data rate of the streams fluctuates randomly around 42 and 21 Mbps mean values, respectively, 
but the links in the network on average are not saturated. Later on, we configure the traffic shaper to limit the data rate to the nominal values of 42 and 21 Mbps for the data streams.
Here is the traffic configuration:

.. the application level in the client. **TODO** 42/21

.. literalinclude:: ../omnetpp.ini
   :start-at: client applications
   :end-before: outgoing streams
   :language: ini

Traffic Shaping
+++++++++++++++

In the client, we want to classify packets from the two packet sources into two traffic classes: best effort and video. 
To do that, we enable IEEE 802.1 stream identification and stream encoding by setting the :par:`hasOutgoingStreams` parameter to ``true``.
We configure the stream identifier module in the
bridging layer to assign the outgoing packets to named streams by their UDP destination port.
Then, the stream encoder sets the PCP number on the packets according to the assigned stream name (using the IEEE 802.1Q header's PCP
field):

.. Then, the client encodes and the switch decodes the streams using the IEEE 802.1Q PCP
   field.

.. We insert the credit based shapers 

   We need to assign the packets created by the two applications to different traffic classes. 

   so

   - we need to assign packets to different traffic classes
   - so the shaper can tell them apart -> classify them and send them to the appropritate queues and create the priority -> there is no priority before the shaper
   - we use the time aware shaper with default schedules (always open gates) and creditbased shapers added
   - but the classification is the same as in the time-aware shaping showcase -> by PCP number

.. literalinclude:: ../omnetpp.ini
   :start-at: outgoing streams
   :end-before: egress traffic shaping
   :language: ini

.. **V1** The traffic shaping takes place in the outgoing network interface (eth1) of the switch
   where both streams pass through. We enable egress traffic shaping in the interface. We specify to traffic classes, 
   and configure the transmission selection algorithmThe traffic shaper limits the data rate of the
   best effort stream to 40 Mbps and the data rate of the video stream to 20 Mbps.
   The excess traffic is stored in the MAC layer subqueues of the corresponding
   traffic class.

.. **V2** The traffic shaping takes place in the outgoing network interface (eth1) of the switch
   where both streams pass through. We enable egress traffic shaping in the interface. This adds :ned:`Ieee8021qTimeAwareShaper` modules
   to the MAC layer of the interface. Then, we specify two traffic classes, 
   and configure the transmission selection algorithm in the time-aware shaper to be :ned:`Ieee8021qCreditBasedShaper`. 

Traffic shaping takes place in the outgoing network interface of the switch (eth1).
The traffic shaper in the switch classifies packets by their PCP number, and
limits the data rate of the best effort stream to 41 Mbps and the data rate of the video stream to 21 Mbps.
The excess traffic is stored in the MAC layer subqueues of the corresponding
traffic class.

.. **TODO** 42 and 21 everywhere

.. **V3** 

.. **TODO** why is this here? i mean its needed but not necessarily here

.. note:: We'll use time-aware shaper modules in the interface's MAC layer to add the credit-based shapers to (as described earlier, this is a convenient way
   to add credit-based shapers to interfaces, as the time-aware shaper provides the necessarly modules, such as queues and classifiers; also, we don't
   configure gate schedules, so there is no time-aware shaping).

We enable egress traffic shaping in the interface (this adds the time-aware shaper module). Then, we specify two traffic classes in the time-aware shaper,
and set the transmission selection algorithm submodule's type to :ned:`Ieee8021qCreditBasedShaper` (this adds two credit-based shaper modules, one per traffic category). 
We configure the idle slope parameters of two credit-based shapers to 42 and 21 Mbps:

.. literalinclude:: ../omnetpp.ini
   :start-at: egress traffic shaping
   :language: ini

.. Thus, the traffic shaper limits the data rate of the
   best effort stream to 42 Mbps and the data rate of the video stream to 21 Mbps.
   The packets are stored in the two queues of the time-aware shaper until ready to be sent.

.. The excess traffic is stored in the MAC layer subqueues of the corresponding
   traffic class.

.. **TODO** eth[1] instead of eth[*] try -> tyf doesnt check out

Results
-------

Let's take a look at how the traffic changes in the network nodes. First, we compare the data rate of the client application traffic
with the shaper incoming traffic for the two traffic categories:

.. figure:: media/client_shaper.png
   :align: center

The traffic is very similar. The shaper incoming traffic has a bit higher data rate due to protocol overhead, which was not yet present in the applications. Also, the two packet streams are mixed in the client's network interface, which can delay packets. So even if the protocol overhead would be accounted for, the traffic would not be the same.

Now let's examine how the traffic changes in the shaper. We compare the data rate of the incoming and outgoing traffic in the shaper:

.. figure:: media/shaper_both.png
   :align: center

The incoming traffic fluctuates around the specified nominal data rate. The outgoing traffic is limited to the nominal data rate.

.. note:: The data rate specified in the ini file as the idle slope parameter is the channel data rate, but this is somewhat different from the outgoing data rate in the shaper due to protocol overhead (PHY + IFG). In this chart, we measure data rate in the shaper, thus we displayed the nominal data rate calculated for the shaper. 

The next chart compares the shaper outgoing and server application traffic:

.. figure:: media/shaper_server.png
   :align: center

Similarly to the first chart, the traffic is similar, with the shaper traffic being higher due to protocol overhead.
The traffic only changes significantly in the shaper, other parts of the network have no traffic shaping effect, as expected.

.. **TODO** thus it only changes in the shaper

The following sequence chart displays frame transmissions in the network. The best effort traffic category is colored blue, the
video red:

.. figure:: media/seqchart.png
   :align: center

.. figure:: media/seqchart2.png
   :align: center

The traffic is more bursty when it arrives in the switch. The traffic shaper in the switch distributes packets evenly,
and interleaves video packets with the best effort ones.

**TODO** reducing burst per-traffic-class

.. The first diagram shows the data rate of the application level outgoing traffic
   in the client. The data rate varies randomly over time for both traffic classes
   but the averages are the same.

.. .. figure:: media/ClientApplicationTraffic.png
      :align: center

.. The next diagram shows the data rate of the incoming traffic of the traffic
   shapers. This data rate is measured inside the outgoing network interface of
   the switch. This diagram is somewhat different from the previous one because
   the traffic is already in the switch, and also because it is measured at a
   different protocol level.

.. .. figure:: media/TrafficShaperIncomingTraffic.png
      :align: center

.. The next diagram shows the data rate of the already shaped outgoing traffic of
   the traffic shapers. This data rate is still measured inside the outgoing network
   interface of the switch but at a different location. As it is quite apparent,
   the randomly varying data rate of the incoming traffic is already transformed
   here into a quite stable data rate.

.. .. figure:: media/TrafficShaperOutgoingTraffic.png
      :align: center

The next diagram shows the queue lengths of the traffic classes in the outgoing
network interface of the switch. The queue lengths don't increase over time because
the data rate of the shaper incoming traffic is, on average, higher than
the data rate of the outgoing traffic. This is because the incoming data rate fluctuates
around the nominal value, i.e., sometimes it is higher. However, the shaper limits the data rate
to the nominal value, so excess packets are stored in the queues, and not dropped.

**TODO** on average dont increase over time cos on average they are the same

.. figure:: media/TrafficShaperQueueLengths.png
   :align: center

The next chart shows the number of credits as it fluctuates rapidly.
It can grow above zero if the corresponding queue is not empty.

**TODO** nem gyujt burst tartalekot ha nincs csomag -> is this needed?

.. figure:: media/TrafficShaperNumberOfCredits.png
   :align: center

The next chart shows the gate states for the two credit-based shapers, and the transmitting
state of the outgoing interface in the switch. Note that the gates can be open for
periods longer than the duration of one packet transmission if the number of credits
is zero or more.
The transmitter is not transmitting
all the time, as the outgoing data rate of the switch (~63Mbps) is less than the channel capacity (100Mbps).

.. figure:: media/TransmittingStateAndGateStates.png
   :align: center

The next diagram shows the relationships between the number of credits, the gate
state of the credit based transmission selection algorithm, and the transmitting
state of the outgoing network interface for the both traffic classes. The diagram
shows only the first 2ms of the simulation to make the details visible:

.. - this is the first 2ms of the simulation
   - the queue length is zero most of the time because if the queue is empty, an incoming packet gets transmitted immediately.
   - so a queue length of zero can conincide with a packet being transmitted (in the same traffic category)
   - in the transmitter, sometimes two packets (each from a different category) are being trasmitted back-to-back,
   with just an Interframe Gap period in between them - e.g. the first two transmissions
   - in the video credit count diagram, first the credit count is 0. then a packet arrives at the queue and starts trasmitting, and the credit count
   decreases. when the transmission is finished, the credit count begins to increase. before it reaches 0, another packet arrives in the queue.
   it cannot be transmitted immediately as the credit count is still negative (so the queue length is 1).
   when the credit count reaches 0, it starts transmitting, and the credit count decreases again

.. figure:: media/TrafficShaping.png
   :align: center

Note that the queue length is zero most of the time because the queue length doesn't increase to one if an incoming packet can be transmitted
immediately. Also, in the transmitter, sometimes two packets (each from a different category) are being trasmitted back-to-back,
with just an Interframe Gap period in between them - e.g. the first two transmissions. This doesn't cause per-traffic-class bursting because
the two packets are of different traffic classes.

We can observe the operation of the credit-based shaper in this diagram. Take, for example, the number of credits for the video traffic category.
First the number of credits is 0. Then a packet arrives at the queue and starts trasmitting, and the number of credits
decreases. When the transmission is finished, the number of credits begins to increase. Before it reaches 0, another packet arrives in the queue.
It cannot be transmitted immediately as the number of credits is still negative (so the queue length is 1).
When the number of credits reaches 0, transmission starts, and the number of credits starts decreasing again.

.. The last diagram shows the data rate of the application level incoming traffic
   in the server. The data rate is somewhat lower than the data rate of the
   outgoing traffic of the corresponding traffic shaper. The reason is that they
   are measured at different protocol layers.

.. .. figure:: media/ServerApplicationTraffic.png
      :align: center

Sources: :download:`omnetpp.ini <../omnetpp.ini>`

Discussion
----------

Use `this <https://github.com/inet-framework/inet/discussions/800>`__ page in the GitHub issue tracker for commenting on this showcase.

