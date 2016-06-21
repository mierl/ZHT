@0xda8cc01ab73b69b8;

struct ZUMessage {
	nRequest @0 :UInt16; # number of requests in this message
	listMsg @1 :List(KVRequest); # for batching and collection operations.

}

struct KVRequest {
	key @0 :Text;
	val @1 :Text;
	opcode @2 :UInt8; #operation code
}
