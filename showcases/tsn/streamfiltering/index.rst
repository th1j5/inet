Per-Stream Filtering and Policing
=================================

Per-stream filtering and policing provides protection against bandwidth violation,
malfunctioning devices, network attacks, etc. Filtering and policing decisions are
made on a per-stream, per-priority, per-frame, etc. basis using various metering
methods.

.. Filtering and policing is performed by layer 2. In INET, 

In INET, filtering and policing is performed in the bridging layer of network nodes.
By default, :ned:`EthernetSwitch` has a simple briding layer module that only does forwarding of frames.
Filtering and policing requires the modular briding layer module (:ned:`BridgingLayer`), which
the TSN-specific extension of :ned:`EthernetSwitch`, the :ned:`TsnSwitch` module, has by default.
Then, filtering and policing can be enabled in the switch with the :par:`hasIngressTrafficFiltering` parameter.

.. This adds a :ned:`StreamFilterLayer` module to the briding layer.

.. In an Ethernet LAN, network devices are connected on L2 by network bridges. In INET, The bridges are basically Ethernet switches,
   and the bridging is done in the switch's bridging layer. The bridging layer forwards packets in the LAN on L2, in constrast
   to L3 (IP) routing, which routes packets between LANs.

   Packets arrive on the switch's incoming interface, and are sent up the protocol stack to the bridging layer.
   The bridging layer makes the decision as to which interface to forward the packet on. Then the packet goes down the
   protocol stack to the outgoing interface.

   **TODO** this, less simplistically

   The bridging layer can include various functionality, such as filtering and policing. **TODO** what else?

The following showcases demonstrate per-stream filtering and policing:

.. toctree::
   :maxdepth: 1

   tokenbucket/doc/index
   statistical/doc/index
   underthehood/doc/index

