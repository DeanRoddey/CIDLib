#  CIDLib - A C++ Development Platform

**What It Is**

CIDLib is a C++ development platform. For many C++ developers a point of reference would be something along the lines of the Qt product perhaps, i.e. a comprehensive set of general purpose tools and classes for creating full featured C++ applications. A primary difference is that CIDLib is not based on the standard C++ libraries (more on that below.) Also, very importantly, CIDLib is not really a library you would incorporate into your code base, it's a lot closer to a 'virtual operating system' that you build a code base on top of.

CIDLib contains about 1100 classes (~450,000 lines of code.) Here is a rough list of the types of functionality it provides:

- Build Tools, project definition system, resource compiler, loadable resources system
- 'Virtual Kernel' platform portability layer
- Full set of standard libraries including streams, collections/cursors, memory buffers, strings, events, mutexes, sockets, threads/processes, serial ports, URLs, TCP/IP, UPnP, secure sockets, audio streaming, TTS/speech recognition, digital audio extraction, medatada extraction, text transcoding, logging, publish/subscribe, image management framework, chunked files, ODBC, etc...)
- Custom implementations of various standards such as PNG, zLib, WebSockets, HTTP, SMTP, Blowfish, AES, MD5, SHA-X, regular expressions, XML, JSON, etc...
- A fairly extensive windowing and 'regular graphics' framework (no 3D graphics yet)
- A very powerful ORB (Object Request Broker), IDL compiler, and a set of standard servers (name, logging, and configuration.) The IDL compiler also supports very advanced C++ enumeration support.
- An embeddable, virtual machine-based, OO language engine called CML (parser, VM, runtime libraries)
- An embeddable IDE for CML editing and debugging
- A simple but nice object store engine (for things like application configuration storage and such, als used by the configuration server mentioned above.)
- A nice test framework
- A file packaging system

**Portability**

CIDLib is primarily developed on Windows, but it has always been designed to be portable, since it is really a virtual OS sort of system and has a full platform abstraction layer. So everything above that is built purely in terms of our own pure C++ interfaces, which makes them highly portable. The lowest level, CIDKernel, is split into interfaces plus optional platform drivers. There are currently platform drivers for Windows and Linux.

However, the Linux one was left unused for a long time and I've only recently begun to rehabilitate it. I've made good progress on it, but I'm not a Linux guy and I could definitely use some help on it. I have it to the point that the build tool is building on Linux and I can build CIDKernel and the lowest level test of CIDKernel, and I'm now working through that to get the core functionality working on Linux.

Not all of the per-platform functionality is in CIDKernel. There are a handful of other libraries that provide some less fundamental functionality that is still, at least for the immediate term, going to require per-platform support. They are similarly structured with an interface and per-platfrom implementations. These don't currently have any Linux side implementation. The secure sockets one is a linchpin currently, since it is blocking a number of important higher level facilities from becoming available on Linux.

The UI code is another story, but just getting the back end functionality cleanly supported on both platforms would be a very powerful thing. Note that the UI code is similarly encapsulated, but there's a lot more to UI portability than that.


**Documentation**

For now, the code is mostly undocumented, other than in the code itself. The build instructions are in the above Wiki pages, see Building below. I've got a documentation compiler done, which reads in XML content and can spit it out in whatever format desired, currently HTML. I've started work on the documentation but that will be quite a job to complete it.

In the meantime, I had already begun to make some videos about some of the technologies I've created as part of this project. Those are on my personal Youtube channel here:

https://www.youtube.com/channel/UCAxCoATHgFBvqZhxFRnZsfw

This video in particular would be a good start (Creating a 'Virtual Kernel' platform abstraction layer)

https://www.youtube.com/watch?v=seXk3RbAjNU.

It is obstensibly about the virtual kernel, but also provides a good overview of the functional boxes and how they are layered and such. The other videos there also very relevant, though some are prior to recent modernization efforts. In particular the ones on enumerations and the ORB was prior to a huge reworking of the code base to use newer style 'enum class' enums. Otherwise they are still relevant.

I've set up a sub-Reddit for discussion and questions, which is here:

https://www.reddit.com/r/CIDLib/


**Building**

As mentioned above, the build instructions are in the Wiki pages of this repository. The rest of the documentation (as it gets created) will be in the actual repository. But you'd need to build CIDLib first before you can then generate the documentation. So the build instructions need to be separate.

CIDLib isn't hard to build because it has its own build system. There are no crazily complex make files or anything. Currently there's one simple make file to build the build tool itself. After that it's all done via the CIDBuild utility. It currently is setup for Visual C++ 2019 on Windows and G++ on Linux. But the build tool uses a pluggable tools interface and can support other tool sets as well.

Let me know if you have any questions or issues with the build instructions.


**The CQC Automation Platform**

CIDLib underlies an even larger project of mine called CQC - The Charmed Quark Controller, which is a very powerful home automation platform. CQC is also now open source. Even if you don't use it, you might find it interesting as well. And seeing what CQC can do will perhaps give you a feeling for the power of CIDLib.

https://www.charmedquark.com

There are lots of CQC tutorial videos on the Charmed Quark Youtube channel:

https://www.youtube.com/user/CharmedQuarkSystems


**Acknowledgements**

Though CIDLib very much tries to avoid use of any third party code beyond the OS, it does use the following:

- Scintilla - A version of the Scintilla engine is wrapped and used as the CML language source editor. Your own CIDLib based applications wouldn't need it unless they used the embedded CML IDE.
- Standard JPEG Libaries - Some of the guts of the standard JPEG libraries are wrapped to provide the JPEG file format support.

Otherwise, on Windows, it is just OS APIs and optional OS SDKs (speech platform, Windows Media Format, etc...) On Linux it will require the installation of various common libraries of course.



**Background**

CIDLib and the CQC automation system that is built on it is the product of decades of work by the author, representing about 50 man-years of work taken together. The very earliest roots go back to around 1992 on OS/2, and the first C++ compiler I had access to. I started writing a string class and it was all downhill from there. The two projects combined are currently around 1.1 million lines of code.

Some people seeing this will immediately start ranting about 'not invented here syndrome', but it's nothing of the sort. My personal interests are in general purpose framework development, so the whole point of it was to do this. It's what I enjoy. And once you have experienced C++ within the context of a unified, coherent system, you really see what it should have become.


**Gotchas**

Of course, in a code base this large, even someone who consumes as much caffeine as myself cannot delve super-deeply into every aspect of every sub-system. So obviously some of the sub-systems could be fleshed out by others with specific interests in those areas. However, as stated in the goals section below, it should not become about itself, and become so baroque that no one can understand it. So it's not even a goal for every sub-system to be taken to the Nth degree and become incomprehensible for all but specialists. It should be about the whole being greater than the sum of the parts.

There will be a few old classes laying around that were created long ago and not touched since, because there's been no need for them all this time. Or the need for them went away. So, don't jump to conclusions that the system is garbage if you happen on one of them. There are some classes in the CIDMath facility of this type. The vector/matrix classes are just some simple bits that used to be used by a raytracing library I had at one point, for instance, and the value mapping classes might not be worth keeping.

Keep in mind that CIDLib must continue to support CQC, which is likely to always be far and away the largest code base built on CIDLib, though I'd love for that to not be the case at some point. So the rate and type of changes allowed will be moderated by that requirement. Of course I'd very much like to move CQC forward quickly to take advantage of improvements. But it's a large and complex product and there's a limit as to how much change can be absorbed without getting dangerous. Change at the periphery will always be more digestable, e.g. new sub-systems that CQC doesn't depend on.


**Goals**

The primary goal of CIDLib is to provide an alternative to the status quo for those who might be interested. I understand of course that the status quo is the status quo because almost everyone lives in that neighborhood. But, some people may like to have an alternative, and this is one.

Not being based on the standard C++/STL libraries, CIDLib is very much a different sort of beast from what is common today. It's use of templates is far lighter, which makes it much more debuggable and comprehensible in my opinion. It does not subscribe to the 'modern' notion that OOP or runtime inheritance is somehow bad, it uses both to huge benefit.

And, being a comprehensive system, used to create applications that are not put together from pieces and parts, that also makes it pretty different from the standard thing today. Most people have never experienced this type of system, and some actually get agitated if anyone (aka me) implies that it might be a superior way of working, at least for some folks.

Among the things I'd personally like to see added to CIDLib are:

- 3D graphics/gaming support. There is a behavior tree facility but it's just very basic stuff so far, and only very lightly used in CQC.
- More internet telephony stuff, there's already some early RTP/RTPC work and a skeleton SIP facility ready for when RTP/RTPC are done
- Custom public cryptography system instead of wrapping system libraries
- Same for UPnP. I've already taken over some of the UPnP functionality.
- Possibly an open ended Web Server framework. CQC has a dedicated Web Server, but CIDLib would be an obvious platform to build such complex back end services on for general purpose use.

But of course almost anything would be possible and interesting.


**Non-Legal Niceties**

1. If you use my stuff, please be a mensch and give me appropriate credit
2. If you find bugs, please send them in, in digestable chunks preferably since obviously someone who writes this much code is somewhat time constrained.
4. If you want to contribute large stuff, i.e. new facilities, then be sure to follow the existing style and substance carefully, because I'm an OCD, paranoid, anal-retentive. It would obviously be best to coordinate with me first to make sure no time and effort are wasted.
5. It would be very much a good thing if you didn't install any non-official builds in any system directories of user's machines, where they could be seen by other CIDLib based applications.
