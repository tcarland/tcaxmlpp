

Firewall Rule Generator:
-----------------------

A firewall rule generator using a straightforward rule syntax. This provides a
common method for defining rules leaving the specific device syntax to the
rule generator. For example, Location Zone A, has 4 firewalls, 2 linux based
firewalls and 2 cisco routers. Any overlapping interfaces between any given
firewall indicates that rules must apply appropriately in both locations.

ToDo:
 - everything




Firewall Log Reader (fwlr):
---------------------------

Firewall log parser for parsing log entries and reporting to tnms.

Caches results by prefix and manages api based on cache.
prefix aggregation by default /24 or irrd.


ToDo:
 - move 'FwService' port/service detection to getnameinfo()?
 - whois prefix/addr info via caching irr/whois layer


