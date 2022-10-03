//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package inet.showcases.tsn.combiningfeatures.preemptioncutthrough;

import inet.networks.base.TsnNetworkBase;
import inet.node.contract.IEthernetNetworkNode;
import inet.node.ethernet.EthernetLink;


network PreemptionCutthrough extends TsnNetworkBase
{
    parameters:
        *.eth[*].bitrate = default(100Mbps);
    submodules:
        client: <default("TsnDevice")> like IEthernetNetworkNode {
            @display("p=350,200");
        }
        switch1: <default("TsnSwitch")> like IEthernetNetworkNode {
            @display("p=500,200");
        }
        switch2: <default("TsnSwitch")> like IEthernetNetworkNode {
            @display("p=600,200");
        }
        server: <default("TsnDevice")> like IEthernetNetworkNode {
            @display("p=750,200");
        }
    connections:
        client.ethg++ <--> EthernetLink <--> switch1.ethg++;
        switch1.ethg++ <--> EthernetLink <--> switch2.ethg++;
        switch2.ethg++ <--> EthernetLink <--> server.ethg++;
}

