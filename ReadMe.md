#  CIDLib - A C++ Development Environment

CIDLib is the product of decades of work by the author, about 25 man-years of work. Actually, it's half of the product. The other half is a very extensive and powerful automation system called CQC (www.charmedquark.com). CIDLib is the 'bottom half' of that code base, and is a general purpose C++ development environment, on top of which CQC is built. CQC's code base remains proprietary, but I have chosen to open source the general purpose part, which comprises right around 450,000 lines of code, approximately 1100 classes. The CQC code base is about 625,000 lines, so a bit over a million lines in the whole code base.

CIDLib is not just a library for this or that. It is a complete environment with build tools, project definition language, loadable text system, a virtual kernel to isolate it from the underlying OS, up through a full set of 'standard libraries', wrappers around lots of common functionality, UI framework, Object Request Broker and IDL compiler, VM based OO macro language with IDE, custom implementations of many standards (XML, PNG, ZLib, JSON, SMTP, HTTP, etc...), and lots of other functionality.

In many ways CIDLib is the anti-STL, anti-piecemeal, anti-modernist view of a C++ world. It doesn't use the STL at all. In fact no operating system or language headers are available outside of the virtual kernel and a small number of higher level functionality wrapper libraries. Everything is built in terms of the CIDLib interfaces for reasons of consistency, quality control, and portability.

Of course there are many ways to skin a developer, as the saying goes. There are MANY examples of the other, more common approach out there. CIDLib is just an alternative to that for people who are interested. It's not about throwing together a bunch of disparate bits and bobs to make a program. It's about a tightly integrated, one stop shopping solution for application development. You can't use bits and pieces of CIDLib, you go all in or not at all, because it's all a tightly integrated whole (though of course it is layered, so you can choose to jump in at whatever layer suits your requirements.)

Because it doesn't use the STL and other third party bits, CIDLib can provide a very consistent architecture from bottom to top. Of course if what you need is beyond the scope CIDLib provides, that's fine. CIDLib is very much not about trying to be everything to everyone, and hence often being a huge compromise to all. It's about providing a more elegant alternative for those folks who needs fit within its scope (and that scope is quite broad.)

**Gotchas**

For now, the code is just being put out there as is for people to start to explore. The documentation is going to be a work in progress and is very light right now, though there are samples and various programs that can be used to see how it all works in the meantime. And I'd already begun to make some videos exploring the system and some of the technologies involved. This one in particular would be a good start:

https://www.youtube.com/watch?v=seXk3RbAjNU

It is obstensibly about the virtual kernel, but also provides a good overview of the functional boxes and how they are layered and such. The other videos here on my personal Youtube channel are also very relevant, though some are prior to recent modernization efforts.

The initial goal is to get the build process documented of course, plus high level documentation of the roadmap sort to get people aware of what's available and where it is and how it fits together and so forth. After that, detailed documentation can be dived into.

The documentation will be written in XML and is built using a custom compiler. The same sort of scheme is used for the CQC documentation and it works pretty well. You will be able to build your own local version of the docs once you have CIDLib built, which you should use when possible since it obviously will match the version you have. But I'll also keep a public copy of what's done so far at the linke below, since folks need a starting place to learn how to set up the environment and build it.

[to be done once there's something to put up]

Of course, in a code base this large, even someone who consumes as much caffeine as myself cannot delve super-deeply into every aspect of every sub-system. So obviously some of the sub-systems could be fleshed out by others with specific interests in those areas. However, as stated in the goals section below, it should not become about itself, and become so baroque that no one can understand it. So it's not even a goal for every sub-system to be taken to the Nth degree and become incomprehensible for all but specialists.

Keep in mind that CIDLib must continue to support CQC, which is likely to always be far and away the largest code base built on CIDLib, though I'd love for that to not be the case at some point. So the rate and type of changes allowed will be moderated by that requirement. Of course I'd very much like to move CQC forward quickly to take advantage of improvements. But it's a large and complex product and there's a limit as to how much change can be absorbed without getting dangerous. Change at the periphery will always be more digestable, e.g. new sub-systems that CQC doesn't depend on or only lightly so.

**Portability**

Though CIDLib is currently only fielded on Windows, keeping it portable has always been a goal. I have done plenty of cross platform development in the past and understand the issues fairly well. As mentioned above, CIDLib is based on a 'virtual kernel' that abstracts it from the OS. That virtual kernel is in turn split into interface plus per-platform implementations. The build tools understand this situation so that helps a lot as well.

There is a Win32 implementation currently. But there is still an old Linux implementation from a few decades ago that's still down there. It would need to be brought up to speed, but it should not be difficult to get almost all of the back end stuff buildable onto Win32 and Linux (32 bit.) And that's not in the 'conditional code all over the place' sort of way, but cleanly supporting both platforms, since 95% of the non-UI code is platform independent and written in terms of our own classes.

There are still a handlful of libraries that need to be given the 'split' treatment, i.e. break out the platform specific stuff from the platform independent parts. I will be giving those attention soon. Until then they are marked for build only on Win32.

The UI code is another story, but the important thing is getting the back end functionality on both platforms. Not that the UI code is not similarly encapsulated, but there's a lot more to UI portability than that.

**Modern vs. Classical**

This code base ultimately goes back to 1992, when I first started exploring C++, and sat down to write a string class. It has been moved forward through many changes. But, being a single author and single client code base, it has not suffered the violences that many code bases this size and of this age might have. It's still quite clean.

I am personally flabbergasted at some of the modernist C++ views and have no desire for CIDLib to go in those directions. Having said that, a lot of 'sane modernization' work has been done on the code base recently. And many sweeps have been made through the code to make use of these changes. But of course in a code base this large it'll still take a while to complete that process. Obviously any new code can take advantage from day one.

But CIDLib remain to one degree or another the anti-STL. Modernization will be done where it really makes a difference to compile time safety and doesn't introduce huge annoyances. It will not be turned into a single giant template with utterly incomprehensible errors, and it won't be turned into Javascript Jr. via use of 'auto' everywhere. Explicit expression of intentions will always be a watch-word in this code base. CIDLib is not for quick and dirty programming, it's for serious application and back end work.

Given its size, it will lean more towards keeping as much non-trivial code out of line as possible, rather than moving almost everything inline or completely header based subsystems, as seems to be the vogue amongst some these days. Build times would be brutal otherwise every time something remotely fundamental is changed.

And we are manly enough not to be frightened by the existence of raw pointers, at least down in the more fundamental code certainly. At higher levels a move towards more smart encapsulation has already begun. But the lower level code needs to be well optimized (though not unduly so) so that higher level code and afford to be less so. There are also lots of bootstrapping issues involved that play into this.

Certainly we will not be trying to turn C++ into a non-OO language either, as many people these seem to be intent on doing.

**Goals**

So the goal of CIDLib is not to see how much third party stuff can be shoved into it, or how 'modern' it can be. It is not a goal to try to make it digestable by people who haven't mastered coding on the medium to larger scale, which clearly a system like CIDLib is more oriented towards. It will not likely ever be a tool for people who want C++ to become a new Javascript.

It is definitely a goal to create a highly consistent, tightly integrated world of bespoke implementations of many technologies, not wrappers around third party stuff and all the complications and compromises that entails. I would hope that other highly experienced developers who have interests in specific areas might get involved and provide implementations of technologies that they are expert in.

It is not a goal to add functionality for the sake of it. It needs to be high quality and highly consistent with the style and substance of the large body of existing code. So no quick hacks of third party code, which would be somewhat difficult anyway given that we don't allow standard library/STL code.

Among the things I'd like to see are 3D graphics/gaming support, and there's an 'advanced graphics' skeleton library ready for the 3D graphics stuff to be done in. More internet telephony stuff, there's already early some RTP/RTPC work and a skeleton SIP facility ready for when RTP/RTPC are done. Maybe to get away from having to wrap system implementations of public key crypography stuff and get a portable, bespoke implementation. And similarly for UPnP, which I would have done myself already if not for lack of time. I've already had to take over some functionality to get around issues in Window's UPnP implementation.

**Usage/License**

The following usage/license terms are imposed:

1. Do whatever you want
2. If you use my stuff, be a mensch and give me appropriate credit
3. If you find bugs, please send them in, in digestable chunks.
4. If you want to contribute large stuff, i.e. new facilities, then be sure to follow the existing style and substance carefully, because I'm an OCD, paranoid, anal-retentive. It would obviously be best to coordinate with me first to make sure no time and effort are wasted.
5. Please don't install any non-official builds in any system directories of user's machines, where they could be seen by other CIDLib based applications. Limit them to your own application's local use.
