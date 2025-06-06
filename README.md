The tcaxmlpp Library
======================

Copyright (c) 2008-2025 Timothy Charlton Arland <tcarland@gmail.com>

# LICENSE

**tcaxmlpp** is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

**tcaxmlpp** is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with **tcaxmlpp**.
If not, see <http://www.gnu.org/licenses/>.

# Overview

This library provides a (C++) object-oriented interface to libxml2's
'Tree API' (DOM) for XML document manipulation.

The *XmlDocument* class provides an interface to parsing any well-formed
XML fragment and acts as a container to the tree and the root XmlNode
object. The *XmlNode* class provides an interface for interacting with
node attributes and any of its children.

## Requirements

- libxml2-dev : just about any version will work. Note the -dev package
  to include the library headers needed to build.
- tcamake : build system definition in *./tcamake*


## Building

To build the library, tcamake is needed for the Makefile bindings.
This can be obtained by cloning the git repo in the parent directory.
The project has been testing with c++11, 17, and 23 using
Gnu g++ 11.4.0.
```sh
cd ..
git clone https://github.com/tcarland/tcamake.git
```

Then simply run 'make' to build the library.


## Contact

Use the issues board for this project as linked above.

---

## Example 1:

It is worth noting that the default *XmlNode* object does not hold a pointer
to its parent *XmlDocument* container. This can complicate node handling
in certain cases, but is done intentionally to simplify general usage.

Since the library 'wraps' libxml2, the user must be wary of the underlying
document to which a node belongs. In most situations, the *XmlDocument* and
*XmlNode* classes are used as is, however, it is possible to inherit and
customize *XmlNode* objects using the *XmlNodeFactory*.

As an example, the provided XmlDocNode class demonstrates a derived *XmlNode*
object holding a pointer to its parent *XmlDocument* container.

Note that this example does not handle validating that the underlying
document ptr is the same when moving/attaching nodes between documents. This
would require deriving the parent document class and overriding
*XmlDocument::attachNode()*.

## Example 2:

This example finds a given node with a specific attribute name and value.
This also demonstrates initializing documents from file or string.
```cpp
@code
#include <stdexcept>
#include <iostream>
#include <string>

#include "XmlDocument.h"
using namespace tcaxmlpp;

int main()
{
    XmlDocument   doc;
    XmlNode      *root   = NULL;
    XmlNode      *mynode = NULL;

    if ( ! doc.initDocument("myxmlfile") )
        throw std::runtime_error("Error: " + doc.getErrorStr());

    root  = doc.getRootNode();

    if ( root == NULL )
        throw std::runtime_error("Error in document, not well formed.");

    mynode = XmlNode::RecursiveFindNode(root, "mynodename", "myattr", "myattrval");

    if ( mynode == NULL )
        throw std::runtime_error(
            "node 'mynodename' with attribute 'myattr=myattrval' not found");

    std::string  xmlblob = doc.NodeToString(mynode);
    std::cout << "some xml blob:\n" << xmlblob << std::endl;

    XmlDocument doc2(xmlblob.c_str(), xmlblob.length());
    doc2.writeFile("somedoc.xml");

    return 0;
}
@endcode
```
