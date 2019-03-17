# ORB 1 Sample

This sample is a basic demonstration of the use of the ORB (Object Request Broker) in a more realistic way than in the ORB related samples in the Basic section. This one creates a server that is a very simple faux data server that serves up 'patient records', and a client that lets you do a couple simple queries against the server for records that match some criteria.

**Environment**

This guy uses the ORB in a more realistic way so it needs to have the name server up, and generally you'd bring up the log server as well. There's a command file (StartORBSrvs.cmd) that you can run which will start up the three standard ORB based servers (name, log, cfg) in a simple command line sort of way. Normally that's how you start them for debug and testing since they are easy to stop (just hit Ctrl-C inside the command window.)

So run that to start up the servers. You'll get a 'is it ok to let this server listen for connections' security prompt for each one the first time. Just let them listen on the local network.

**Server**

Just run it without any parameters (ORB1SampS<enter>) and it will start up and start listening on port 9999. Normally of course you'd get that kind of stuff from config or command line. It will create a server side instance of the demo interface and register it with the ORB, and with the ORB's rebinder. It will then start the rebinding which will publish the interface in the name server. You can run this command:

`CIDNSAdmin Dump`

To see the current name server contents. You should see /CIDLib/ORB1Samp/IntfImpl as one of the bindings. That's the interface that the client will connect to.

To exit, just hit any key.

**Client**

The client gets a proxy to the name server, looks up the server's interface, creates a proxy for that interface, then enters a loop to let you enter some simple query commands. These are:

`findname [name]`
`findstatus [status]`

See the ORB1SampS_IntfImpl.cpp file at the bottom where it loads up some dummy records. That will show you the names and statuses. So do:

`findname Nancy Sinatra`

and that should return the record for that patient.

For status, use the values of the status enumeration, defined in the ORB1SampSh_Types.CIDIDL IDL file. The client will use the standard smart enum stuff to map that value to the corresponding enumeration (case sensitive.) So:

`findstatus Single`

should return all of the records for single patients.
