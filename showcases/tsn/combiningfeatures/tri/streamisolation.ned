//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
package inet.showcases.tsn.combiningfeatures.tri;

import inet.networks.base.TsnNetworkBase;
import inet.node.ethernet.EthernetLink;
import inet.node.tsn.TsnClock;
import inet.node.tsn.TsnDevice;
import inet.node.tsn.TsnSwitch;


network streamisolation extends TsnNetworkBase
{
    parameters:
        *.eth[*].bitrate = default(1000Mbps);
    submodules:
        tsnClock: TsnClock {
            @display("p=550,424");
        }
        tsnDevice: TsnDevice {
            @display("p=350,200");
        }
        switch: TsnSwitch {
            @display("p=550,200");
        }
        server: TsnDevice {
            @display("p=750,200");
        }
        tsnDevice1: TsnDevice {
            @display("p=350,300");
        }
    connections:
        tsnClock.ethg++ <--> EthernetLink <--> switch.ethg++;
        tsnDevice1.ethg++ <--> EthernetLink <--> switch.ethg++;
        tsnDevice.ethg++ <--> EthernetLink <--> switch.ethg++;
        switch.ethg++ <--> EthernetLink <--> server.ethg++;
}
