#  CIDLib - A C++ Development Platform

**What It Is**

CIDLib is a C++ development platform. For many C++ developers a point of reference would be something along the lines of the Qt product perhaps, i.e. a comprehensive set of general purpose tools and classes for creating full featured C++ applications. A primary difference is that CIDLib is not based on the standard C++ libraries (more on that below.)

CIDLib contains about 1100 classes (~450,000 lines of code.) Here is a rough list of the types of functionality it provides:

- Build Tools, project definition system, resource compiler, loadable text system
- 'Virtual Kernel' platform portability layer
- Full set of standard libraries including streams, collections/cursors, memory buffers, strings, events, mutexes, sockets, threads/processes, serial ports, URLs, UPnP, secure sockets, audio streaming, speech recognition, digital audio extraction, medatada extraction, text transcoding, logging, publish/subscribe, PNG, JPEG, bitmaps, chunked files, ODBC, etc...)
- Custom implementations of various standards such as PNG, zLib, WebSockets, HTTP, SMTP, Blowfish, AES, MD5, SHA-X, regular expressions, XML, JSON, etc...
- A fairly extensive windowing and 'regular graphics' framework (no 3D graphics yet)
- A very powerful ORB (Object Request Broker), IDL compiler, and a set of standard servers (name, logging, and configuration.)
- An embeddable, virtual machine-based, OO language engine called CML (parser, VM, runtime libraries)
- An embeddable IDE for CML editing and debugging
- A simple but nice object store engine (for things like application configuration storage and such, als used by the configuration server mentioned above.)
- A nice test framework
- A file packaging system


**Documentation**

For now, the code is just there to explore. I needed to get it uploaded and start working against the remote repository, and figured it made sense to let people go ahead and start looking through it while I get the initial documentation done. The initial goal is to get the build process documented of course, plus high level documentation of the roadmap sort to get people aware of what's available and where it is and how it fits together and so forth. After that, detailed documentation can be dived into.

In the meantime, I had already begin to make some videos about some of the technologies I've created as part of this project. Those are on my personal Youtube channel here:

https://www.youtube.com/channel/UCAxCoATHgFBvqZhxFRnZsfw

This video in particular would be a good start (Creating a 'Virtual Kernel' platform abstraction layer)

https://www.youtube.com/watch?v=seXk3RbAjNU.

It is obstensibly about the virtual kernel, but also provides a good overview of the functional boxes and how they are layered and such. The other videos there also very relevant, though some are prior to recent modernization efforts. In particular the ones on enumerations and the ORB was prior to a huge reworking of the code base to use newer style 'enum class' enums. Otherwise they are still relevant.


**Acknowledgements**

Though CIDLib very much tries to avoid use of any third party code beyond the OS, it does use the following:

- Scintilla - A verion of the Scintilla engine is wrapped and used as the CML language source editor. Your own CIDLib based applications wouldn't need it unless the used the embedded CML IDE.
- Standard JPEG Libaries - Some of the guts of the standard JPEG libraries are wrapped to provide the JPEG file format support.

Otherwise it is just OS APIs and optional OS SDKs (speech recognition, Windows Media Format, etc...)


**The CQC Automation Platform**

CIDLib underlies an even larger project of mine called CQC - The Charmed Quark Controller, which is a very powerful home automation platform. CQC is not open source, it is a proprietary product, though you might find it interesting as well. And seeing what CQC can do will perhaps give you a feeling for the power of CIDLib.

https://www.charmedquark.com

There are lots of CQC tutorial videos on the Charmed Quark Youtube channel:

https://www.youtube.com/user/CharmedQuarkSystems


**Portability**

Though CIDLib is currently only fielded on Windows, keeping it portable has always been a goal. I have done plenty of cross platform development in the past and understand the issues fairly well. As mentioned above, CIDLib is based on a 'virtual kernel' that abstracts it from the OS. That virtual kernel is in turn split into interface plus per-platform implementations. The build tools understand this situation so that helps a lot as well.

There is a Win32 implementation currently. But there is still an old Linux implementation from a few decades ago that's still down there. It would need to be brought up to speed, but it should not be difficult to get almost all of the back-end stuff buildable onto Win32 and Linux (32 bit.) And that's not in the 'conditional code all over the place' sort of way, but cleanly supporting both platforms, since 95% of the non-UI code is platform independent and written in terms of our own classes.

There are still a handful of libraries that need to be given the 'split' treatment, i.e. break out the platform specific stuff from the platform independent parts. I will be giving those attention soon. Until then they are marked for build only on Win32.

The UI code is another story, but just getting the back end functionality cleanly supported on both platforms would be a very powerful thing. Not that the UI code is not similarly encapsulated, but there's a lot more to UI portability than that.


**Background**

CIDLib and the CQC automation system that is built on it is the product of decades of work by the author, representing almost 50 man-years of work taken together. The very earliest roots go back to around 1992 on OS/2, and the first C++ compiler I had access to. I started writing a string class and it was all downhill from there.

Some people seeing this will immediately start ranting about 'not invented here syndrome', but it's nothing of the sort. My personal interests are in general purpose framework development, so the whole point of it was to do this. It's what I enjoy.


**Gotchas**

Of course, in a code base this large, even someone who consumes as much caffeine as myself cannot delve super-deeply into every aspect of every sub-system. So obviously some of the sub-systems could be fleshed out by others with specific interests in those areas. However, as stated in the goals section below, it should not become about itself, and become so baroque that no one can understand it. So it's not even a goal for every sub-system to be taken to the Nth degree and become incomprehensible for all but specialists.

There will be a few old classes laying around that were created long ago and not touched since, because there's been no need for them all this time. Or the need for them went away. Don't, jump to conclusions that the system is garbage if you happen on one of them. There are some classes in the CIDMath facility of this type. The vector/matrix classes are just some simple bits that used to be used by a raytracing library I had at one point, for instance, and the value mapping classes might not be worth keeping.

Keep in mind that CIDLib must continue to support CQC, which is likely to always be far and away the largest code base built on CIDLib, though I'd love for that to not be the case at some point. So the rate and type of changes allowed will be moderated by that requirement. Of course I'd very much like to move CQC forward quickly to take advantage of improvements. But it's a large and complex product and there's a limit as to how much change can be absorbed without getting dangerous. Change at the periphery will always be more digestable, e.g. new sub-systems that CQC doesn't depend on.


**Goals**

The primary goal of CIDLib is to provide an alternative to the status quo for those who might be interested. I understand of course that the status quo is the status quo because almost everyone lives in that neighborhood. But, some people may like to have an alternative, and this is one.

Not being based on the standard C++/STL libraries, CIDLib is very much a different sort of beast from what is common today. It's use of templates is far lighter, which makes it much more debuggable and comprehensible in my opinion. It does not subscribe to the 'modern' notion that OOP is somehow bad, it uses OOP to huge benefit.

And, being a comprehensive system, used to create applications that are not put together from pieces and parts, that also makes it pretty different from the standard thing today. Most people have never experienced this type of system, and some actually get agitated if anyone (aka me) implies that it might be a superior way of working, at least for some folks.

Among the things I'd personally like to see added to CIDLib are:

- 3D graphics/gaming support
- More internet telephony stuff, there's already early some RTP/RTPC work and a skeleton SIP facility ready for when RTP/RTPC are done
- Custom public cryptography system instead of wrapping system libraries
- Same for UPnP. I've already taken over some of the UPnP functionality.

But of course almost anything would be possible and interesting.


**Non-Legal Niceties**

1. If you use my stuff, please be a mensch and give me appropriate credit
2. If you find bugs, please send them in, in digestable chunks perferably since obviously someone who writes this much code is somewhat time constrained.
4. If you want to contribute large stuff, i.e. new facilities, then be sure to follow the existing style and substance carefully, because I'm an OCD, paranoid, anal-retentive. It would obviously be best to coordinate with me first to make sure no time and effort are wasted.
5. It would be very much a good thing if you didn't install any non-official builds in any system directories of user's machines, where they could be seen by other CIDLib based applications.
