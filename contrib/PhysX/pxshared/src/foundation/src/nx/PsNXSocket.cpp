/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "PsFoundation.h"
#include "PsSocket.h"
#include "PsThread.h"
#include "PsArray.h"
#include "foundation/PxMemory.h"

#include <nn/socket.h>
#include <nn/nn_Log.h>

#define SOCKET_ERROR -1

namespace physx
{
namespace shdfnd
{

const PxU32 Socket::DEFAULT_BUFFER_SIZE = 32768;

class SocketImpl
{
public:
	SocketImpl(bool isBlocking);
	virtual ~SocketImpl();

	bool	init();
	bool	connect(const char* host, PxU16 port, PxU32 timeout);
	void	disconnect();
	bool	listen(PxU16 /*port*/)
	{
		return false; // not implemented on this platform
	}
	bool	accept(bool /*block*/)
	{
		return false; // not implemented on this platform
	}

	void	setBlocking(bool blocking);

	virtual PxU32	write(const PxU8* data, PxU32 length);
	virtual bool	flush();

	PxU32			read(PxU8* data, PxU32 length);

	static void* allocate(size_t size) { return PX_ALLOC(size, "Socket"); }
	static void deallocate(void* mem, size_t) { PX_FREE(mem); }

	PX_FORCE_INLINE	bool		isBlocking() const	{ return mIsBlocking; }
	PX_FORCE_INLINE	bool		isConnected() const	{ return mIsConnected; }
	PX_FORCE_INLINE	const char*	getHost() const		{ return mHost; }
	PX_FORCE_INLINE	PxU16		getPort() const		{ return mPort; }

protected:
	int				mSocket;
	const char*		mHost;
	PxU16			mPort;
	bool			mIsConnected;
	bool			mIsBlocking;
};


class BufferedSocketImpl: public SocketImpl
{
public:
	BufferedSocketImpl(bool isBlocking);
	virtual ~BufferedSocketImpl() {};
	bool flush();
	PxU32 write(const PxU8* data, PxU32 length);

private:
	PxU32	mBufferPos;
	PxU8	mBuffer[Socket::DEFAULT_BUFFER_SIZE];
};

BufferedSocketImpl::BufferedSocketImpl(bool isBlocking)
	: SocketImpl(isBlocking)
	, mBufferPos(0)
{}


SocketImpl::SocketImpl(bool isBlocking)
	: mSocket(SOCKET_ERROR)
	, mHost(NULL)
	, mPort(0)
	, mIsConnected(false)
	, mIsBlocking(isBlocking)
{
}


SocketImpl::~SocketImpl()
{
}


bool SocketImpl::init()
{
	mSocket = nn::socket::Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return (mSocket != SOCKET_ERROR);
}


void SocketImpl::setBlocking(bool blocking)
{
	if (blocking != mIsBlocking)
	{
		int mode = nn::socket::Fcntl(mSocket, F_GETFL, 0);
		if (!blocking)
			mode |= O_NONBLOCK;
		else
			mode &= ~O_NONBLOCK;
		int ret = nn::socket::Fcntl(mSocket, F_SETFL, mode);
		if (ret != SOCKET_ERROR)
			mIsBlocking = blocking;
	}
}


bool SocketImpl::flush()
{
	return true;
};


bool SocketImpl::connect(const char* host, PxU16 port, PxU32 timeout)
{
	if (!init()) 
		return false;

	setBlocking(false);

	sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = nn::socket::InetHtons(port);

	// get host part 
	int result = nn::socket::InetPton(AF_INET, host, &socketAddress.sin_addr);
	PX_UNUSED(result);
	PX_ASSERT(result != SOCKET_ERROR);

	if (nn::socket::Connect(mSocket, (sockaddr*)&socketAddress, sizeof(socketAddress)) < 0)
	{
		if (nn::socket::GetLastErrno() != EINPROGRESS)
		{
			disconnect();
			return false;
		}

		//Use poll function call to monitor the connect call.
		pollfd socketDesc;
		socketDesc.fd = mSocket;
		socketDesc.events = POLLOUT;
		socketDesc.revents = 0;
		int pollret = nn::socket::Poll(&socketDesc, 1, static_cast<int>(timeout));
		if (pollret != 1 || (socketDesc.revents & POLLERR) || !(socketDesc.revents & POLLOUT))
		{
			disconnect();
			return false;
		}

		// check if we are really connected, above code seems to return 
		// true if host is a unix machine even if the connection was
		// not accepted.
		char buffer;
		if(nn::socket::Recv(mSocket, (void*)&buffer, 0, 0) < 0)
		{
			if(nn::socket::GetLastErrno() != EWOULDBLOCK)
			{
				disconnect();
				return false;
			}
		}
	}

	setBlocking(mIsBlocking);
	mIsConnected = true;
	mPort = port;
	mHost = host;

	return true;
}


void SocketImpl::disconnect()
{
	if (mSocket != SOCKET_ERROR)
	{
		int result;
		if (mIsConnected)
		{
			setBlocking(true);
			result = nn::socket::Shutdown(mSocket, SHUT_RDWR);
			PX_UNUSED(result);
			PX_ASSERT(result != SOCKET_ERROR);
		}

		result = nn::socket::Close(mSocket);
		PX_UNUSED(result);
		PX_ASSERT(result != SOCKET_ERROR);
		mSocket = SOCKET_ERROR;
	}

	mIsConnected = false;
	mPort = 0;
	mHost = NULL;
}


PxU32 SocketImpl::write(const PxU8* data, PxU32 length)
{
	int sent = 0;
	while((sent = nn::socket::Send(mSocket, (const void*)data, (size_t)length, 0)) == SOCKET_ERROR)
	{
		if(nn::socket::GetLastErrno() != EWOULDBLOCK)
		{
			mIsConnected = false;
			return 0;
		}
	}

	return (PxU32)sent;
}


PxU32 SocketImpl::read(PxU8* data, PxU32 length)
{
	int bytesReceived = 0;

	// If out of receive buffer, increase it
	while((bytesReceived = nn::socket::Recv(mSocket, (void*)data, (size_t)length, 0)) == SOCKET_ERROR && 
		nn::socket::GetLastErrno() == ENOBUFS)
	{
		int iBuffSize = (int)length;

		// terminate the loop if we cannot increase the buffer size
		if(nn::socket::SetSockOpt(mSocket, SOL_SOCKET, SO_RCVBUF, (void*)&iBuffSize, sizeof(int)) != 0)
			break;
	}

	if(bytesReceived <= 0)
	{
		bytesReceived = 0;
		mIsConnected = false;
	}

	return PxU32(bytesReceived);
}


bool BufferedSocketImpl::flush()
{
	PxU32 totalBytesWritten = 0;
	PxI32 bytesWritten = 1;
	while(totalBytesWritten < mBufferPos && bytesWritten > 0)
	{
		bytesWritten = SocketImpl::write(mBuffer+totalBytesWritten, mBufferPos-totalBytesWritten);
		if(bytesWritten > 0)
			totalBytesWritten += bytesWritten;
	}
	bool ret = (totalBytesWritten == mBufferPos);
	mBufferPos = 0;
	return ret;
}


PxU32 BufferedSocketImpl::write(const PxU8* data, PxU32 length)
{
	PxU32 bytesWritten = 0;
	while(length > (Socket::DEFAULT_BUFFER_SIZE - mBufferPos))
	{
		PxU32 currentChunk = Socket::DEFAULT_BUFFER_SIZE - mBufferPos;
		PxMemCopy(mBuffer+mBufferPos, data+bytesWritten, currentChunk);
		mBufferPos = Socket::DEFAULT_BUFFER_SIZE;
		if(!flush())
		{
			disconnect();
			return bytesWritten;
		}
		bytesWritten += currentChunk;
		length -= currentChunk;
	}
	if(length > 0) 
	{
		PxMemCopy(mBuffer+mBufferPos, data+bytesWritten, length);
		bytesWritten += length;
		mBufferPos += length;
	}
	if(mBufferPos == Socket::DEFAULT_BUFFER_SIZE)
	{
		if (!flush())
		{
			disconnect();
			return bytesWritten;
		}
	}
	return bytesWritten;
}


Socket::Socket(bool inEnableBuffering, bool blocking)
{
	if (inEnableBuffering)
	{
		void* mem = PX_ALLOC(sizeof(BufferedSocketImpl), PX_DEBUG_EXP("BufferedSocketImpl"));
		mImpl = PX_PLACEMENT_NEW(mem, BufferedSocketImpl)(blocking);
	}
	else
	{
		void* mem = PX_ALLOC(sizeof(SocketImpl), PX_DEBUG_EXP("SocketImpl"));
		mImpl = PX_PLACEMENT_NEW(mem, SocketImpl)(blocking);
	}
}


Socket::~Socket()
{
	mImpl->flush();
	mImpl->disconnect();
	mImpl->~SocketImpl();
	PX_FREE(mImpl);
}


bool Socket::connect(const char* host, PxU16 port, PxU32 timeout)
{
	return mImpl->connect(host, port, timeout);
}


bool Socket::listen(PxU16 port)
{
	return mImpl->listen(port);
}


bool Socket::accept(bool block)
{
	return mImpl->accept(block);
}


void Socket::disconnect()
{
	mImpl->disconnect();
}


bool Socket::isConnected() const
{
	return mImpl->isConnected();
}


const char* Socket::getHost() const
{
	return mImpl->getHost();
}


PxU16 Socket::getPort() const
{
	return mImpl->getPort();
}


bool Socket::flush()
{
	if(!mImpl->isConnected())
		return false;
	return mImpl->flush();
}


PxU32 Socket::write(const PxU8* data, PxU32 length)
{
	if(!mImpl->isConnected())
		return 0;
	return mImpl->write(data, length);
}


PxU32 Socket::read(PxU8* data, PxU32 length)
{
	if(!mImpl->isConnected())
		return 0;
	return mImpl->read(data, length);
}


void Socket::setBlocking(bool blocking)
{
	if(!mImpl->isConnected())
		return;
	mImpl->setBlocking(blocking);
}


bool Socket::isBlocking() const
{
	if(!mImpl->isConnected())
		return true;
	return mImpl->isBlocking();
}

} // namespace shdfnd
} // namespace physx
