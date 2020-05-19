TODO:  Move these to issues on github, if they are still relevant

# Testing of syncs
There is one bug that is preventing the testing of one way syncs.

Summary: forkHandleServer(GenSync&, GenSync) doesn't close communicant connection, even if the SyncMethods explicitly
close connection themselves.

Steps to Reproduce: Call forkHandleServer twice with the same pair of GenSyncs using CommSockets. The first call will
succeed normally, whereas the second call will fail with a port-in-use error.

Expected Results: The server should close connection after syncing with client.

Actual Results: The server remains listening for connections, thereby blocking other GenSyncs from listening on the
same connection (eg. ports)

---
More Info: This isn't a bug with the implementation of forkHandleServer; I think it has something to do with how
communicants are stored in memory. My reasoning is that the connection stays open on the communicant until
the server GenSync object is deleted. That is why the normal forkHandle succeeds; the GenSync server is deleted when the
child process ends, thus closing the connection by calling the GenSync destructor. In forkHandleServer, the server's
destructor is not called because it persists after the function call. If the GenSync persists in memory, the connection
remains open.

However, all the SyncMethods implemented so far explicitly close connection at the end of SyncMethod::SyncServer.

I think this might have something to do with the pointers to communicants stored in the GenSync objects.