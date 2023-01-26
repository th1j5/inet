:orphan:

.. _dg:cha:tags

Understanding Tags
==================

Overview
--------

It is often necessary to convey extra information with a packet. For
example, when an application-layer module wants to send data over TCP,
some connection identifier needs to be specified for TCP. When TCP sends
a segment over IP, IP needs a destination address. When IP sends a datagram to an Ethernet interface
for transmission, a destination MAC address must be specified. This
extra information is attached to a packet in the form tags.

Tags
----

Packets have a set of tags attached to them. A tag is usually a very small data structure that focuses on
a single parameterization aspect of one or more protocols.

Tags come in three flavors:

 - requests (are called ``FooReq``) carry information from higher layer protocols to lower layer protocols
 - indications (are called ``FooInd``) carry information from lower layer protocols to higher layer protocols
 - plain tags (are called ``FooTag``) contain some generic meta information
 - base classes (are called ``FooTagBase``) must not be attached to packets

Some notable tag examples:

 - :cpp:`SocketReq`, :cpp:`SocketInd` specifies the application socket
 - :cpp:`L4PortReq`, :cpp:`L4PortInd` specifies the sender and receiver port
 - :cpp:`L3AddressReq`, :cpp:`L3AddressInd` specifies source and destination network addresses
 - :cpp:`InterfaceReq`, :cpp:`InterfaceInd` specifies the outgoing or incoming network interface
 - :cpp:`NextHopAddressReq` specifies the next hop address for packet routing
 - :cpp:`VlanReq`, :cpp:`VlanInd` specifies the virtual LAN identifier of IEEE 802.1Q
 - :cpp:`PcpReq`, :cpp:`PcpInd` specifies the priority code point of IEEE 802.1Q
 - :cpp:`StreamReq`, :cpp:`StreamInd` specifies the TSN stream identifier
 - :cpp:`MacAddressReq`, :cpp:`MacAddressInd` specifies source and destination MAC addresses
 - :cpp:`Ieee80211ModeReq`, :cpp:`Ieee80211ModeInd` specifies the IEEE 802.11 PHY mode
 - :cpp:`Ieee80211ChannelReq`, :cpp:`Ieee80211ChannelInd` specifies the IEEE 802.11 channel
 - :cpp:`SignalPowerReq`, :cpp:`SignalPowerInd` specifies send and receive signal power

 - :cpp:`DispatchProtocolReq`, :cpp:`DispatchProtocolInd` specifies intended receiver protocol module inside the network node
 - :cpp:`EncapsulationProcotolReq`, :cpp:`EncapsulationProcotolInd` specifies the protocol header encapsulation order 
 - :cpp:`PacketProtcolTag` specifies the protocol of the packet contents
 - etc.

Communicating Through Protocol Layers
-------------------------------------

Tags can pass through protocol layers and reach far away from the originator
module in both the downward and upward direction. In general, tags are removed
where they are processed, usually turning the tag into some header fields in a
packet. Of course, protocols are free to ignore any tag they wish based on their configuration and
state.

Tags are not transmitted from one network node to another. All physical layer protocols are required to delete all tags
(except the :cpp:`PacketProtocolTag`) from a packet before sending it to the peer or
the medium. In other words, tags are only meant to be processed in the same
network node.






The Protocol of a Packet
------------------------

The :cpp:`PacketProtocolTag` describes the

DispatchProtocolReq
-------------------

TODO

EncapsulationProtocolReq
------------------------

TODO



add examples

DispatchProtocolReq vs MessageDispatcher only looks at DispatchProtocolReq
 next hop routing inside a network node
 - can be hardcoded in C++
 - can be set via module parameter
 - can be determined by module state
 - can be determined by packet data or packet meta-data
 - can be requested by EncapsulationProtocolReq

dispatcher architecture can be
 - no dispatcher
 - star topology
 - layered topology
 - combination of the above

items
 - protocols must be registered by modules
 - the same protocol cannot be registered on the same dispatcher more than once 
 - without DispatchProtocolReq the module must be directly connected to the next protocol module
 - dispatch service request, service confirmation, protocol indication, protocol response
 - application socket dispatches to the protocol corresponding to the socket (e.g. Udp) 
 - every protocol must correspond to a single module from the output gate where the packet was sent out
 - Tcp dispatches to Ipv4 based on the destination address format
 - registered protocols in MessageDispatchers
 - DispatchProtocolInd from where the packet came

EncapsulationProtocolReq
------------------------
EncapsulationProtocolReq vs protocol headers on a packet for further processing
this is a list, it doesn't have to be complete, other headers can be added too
there are indications too
where packets go inside a network node

Ipv4 inserts Ethernet into the EncapsulationProtocolReq based on the protocol a network interface understands

EncapsulationProtocolReq can be ignored, can be transformed, can be taken into account, can be removed, etc.

policy vs infrastructure
who decides what?

EncapsulationProtocolInd is filled with protocols as the packet is being processed

PacketDirectionReverser (forwarding policy) says the packet should go out with the same headers it came in
EncapsulationProtocolInd is turned into EncapsulationProtocolReq (default policy decision)
other policies can change Reqs (e.g. VlanReqMapper)

encapsulation protocol req can be prepended (most often) or appended (network interface protocol)

Default bridging layer forwarding policy
----------------------------------------
other meta-data are also carried as indications and turned into requests with the default forwarding policy
inbound -> outbound packet transformation

peldak:
no vlan -> vlan
vlan 1 -> vlan 2
no redundancy -> redundancy
etc.

loosly coupled modules require a more complex architecture

1. how did the incoming packet look like
2. we send the outgoing packet the same way
3. dispatch the packet to the first one in the encapsulation
4. transform the meta-data along the way

examples for transformation

packet tagging (PacketTagger), forwarding policy (PacketDirectionReverser),
IPv4 (Ipv4), interface selector (RelayInterfaceSelector), 
VLAN mapping (VlanReqMapper), stream encoding (StreamEncoder)












Use case:
 - sequence of protocol headers/trailers
   e.g. Application Data, UDP, IPv4, IEEE 802.2 LLC, IEEE 802.15.4
 - sequence of protocol modules
   App, Udp, Ipv4, Ieee8022Llc, Ieee802154
 - dispatch decision (can be hardcoded, locally parametrizable, can depend on packet data or meta data, looked up from database, overridden by being already specified)
   App -> Udp (App)
   Udp -> Ipv4 (Udp by destination address)
   Ipv4 -> Ieee8022Llc (App, Network interface?)
   Ieee8022Llc -> Ieee802154 (Network interface)
   Ieee802154 -> NetworkInterface (Ipv4, App)

determining the next protocol for a packet in a protocol module (DispatchProtocolReq) (outgoing/incoming)
 - can be unspecified
 - can be hard-coded into C source of the module that is currently processing the packet
 - can be specified by a protocol parameter on the module
 - can be determined by a socket
 - can be determined by the address type (e.g. Ipv4, Ipv6) of a destination address
 - can be determined by one or more header fields from an incoming packet using a protocol group mapping
 - can be determined by a header field directly referencing the protocol
 - can be the requested transport protocol, network protocol, MAC protocol (TransportProtocolReq, NetworkProtocolReq, MacProtocolReq)
 - can be the expected protocol of a network interface
 - can be determined by the current packet protocol
 - can be determined by from which protocol module the packet came (e.g. DispatchProtocolInd)
 - can be determined by a module that created or previously processed the packet (e.g. application by EncapsulationProtocolReq)
 - can be the first protocol in encapsulation protocol request (EncapsulationProtocolReq)
 - can be determined by some other packet meta-data (e.g. EncapsulationProtocolReq?)

what should we do when the payload protocol is not specified in the packet (e.g. 802.15.4 or 802.11)?
this information should be attached as a meta-data to the packet by the module which knows this, so a packet dissector (or printer) can process the packet without context

documentation goes into Developer's Guide

who adds DispatchProtocolReq?
 - the module who determines the next protocol
who queries DispatchProtocolReq?
 - the message dispatcher module
who removes DispatchProtocolReq?
 - the DispatchProtocolReq destination module who processes the packet

how is the EncapsulationProtocolReq used now?
  RelayInterfaceSelector appends network interface protocol into EncapsulationProtocolReq if DispatchProtocolReq is already present
  Ieee8021qTagEpdHeaderInserter removes first protocol from EncapsulationProtocolReq if present to use in DispatchProtocolReq
  Ieee8021rTagEpdHeaderInserter removes first protocol from EncapsulationProtocolReq if present to use in DispatchProtocolReq
  Ipv4 appends network interface protocol to the end of EncapsulationProtocolReq and copies first entry to DispatchProtocolReq
  StreamEncoder delays existing DispatchProtocolReq and inserts 802.1q and 802.1r protocols into EncapsulationProtocolReq depending on what tags are attached
 ? PacketTaggerBase adds EncapsulationProtocolReq with 802.1q if PcpReq is added

what are typical delayed protocol dispatch actions?
 - pop protocol at the beginning of EncapsulationProtocolReq, get first protocol to where the packet must be sent to
 - push protocol at the beginning of EncapsulationProtocolReq to send packet to a specific protocol first
 - push protocol at the end of EncapsulationProtocolReq to send packet to a specific protocol last

fallback mechanism for next protocol:
 - if InterfaceReq is added, push network interface protocol at the end of EncapsulationProtocolReq
 - if L3 destination address is specified, set NetworkProtocolReq to corresponding network protocol
 - pop protocol at the beginning of EncapsulationProtocolReq
 - if no protocol is found, use module specific next protocol parameter
 - if no protocol is specified, use module specific hard-coded next protocol
   - in applications use the protocol from TransportProtocolReq
   - in transport protocols use the protocol from NetworkProtocolReq
   - in network protocols use the protocol from MacProtocolReq
 - if no protocol is hard-coded, leave DispatchProtocolReq unspecified (direct connections decide who processes the packet next)

TSN use case:
App [app] -> udp (udp app) ()
Udp [udp app] -> ipv4 (ipv4 udp app) ()
Ipv4 [ipv4 udp app] -> ethernetmac (ethernetmac ipv4 udp app) ()
StreamIdentifier [ipv4 udp app] -> ethernetmac (ethernetmac ipv4 udp app) ()
StreamEncoder [ipv4 udp app] -> ieee8021r (ethernetmac ieee8021q ieee8021r ipv4 udp app) (ethernetmac ieee8021q)
Ieee8021r [ieee8021r ipv4 udp app] -> ieee8021q (ethernetmac ieee8021q ieee8021r ipv4 udp app) (ethernetmac)
Ieee8021q [ieee8021q ieee8021r ipv4 udp app] -> ethernetmac (ethernetmac ieee8021q ieee8021r ipv4 udp app) ()
EthernetMac [ethernetmac ieee8021q ieee8021r ipv4 udp app] -> network interface (ethernetmac ieee8021q ieee8021r ipv4 udp app) ()

802.15.4 use case:
App [app] -> udp (udp app) ()
Udp [udp app] -> ipv4 (ipv4 udp app) ()
Ipv4 [ipv4 udp app] -> ieee802154 (ieee802154 ipv4 udp app) ()
Llc [ipv4 udp app] -> ieee8022llc (ieee802154 ieee8022llc ipv4 udp app) (ieee802154)
Ieee8022llc [ieee8022llc ipv4 udp app] -> ieee802154 (ieee802154 ieee8022llc ipv4 udp app) ()
Ieee802154 [ieee802154 ieee8022llc ipv4 udp app] -> network interface (ieee802154 ieee8022llc ipv4 udp app) ()

Restructure StandardHost to include a separate LLC layer which contains:
 - a separate packet classifier (LlcClassifier) that decides which LLC is used (if any) based on the MAC protocol for example
 - IEEE 802.2 SNAP LLC
 - IEEE 802 EPD LLC
 - ZIGBEE LLC (https://lucidar.me/en/zigbee/autopsy-of-a-zigbee-frame/)

