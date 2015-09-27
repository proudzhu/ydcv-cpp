#ifndef _CPPLOG_H
#define _CPPLOG_H

#include <iostream>

namespace CPPLOG
{
	enum loglevel_t
	{
		LOG_ERROR	= (1 << 0),
		LOG_WARN	= (1 << 1),
		LOG_INFO	= (1 << 2),
		LOG_DEBUG	= (1 << 3),
		LOG_VERBOSE = (1 << 4),
	};

	class NullBuffer : public std::streambuf
	{
		public:
			int overflow(int c)
			{
				return c;
			}
	};

	class NullStream : public std::ostream
	{
		public:
			NullStream() : std::ostream(&m_sb)
			{ }
		private:
			NullBuffer m_sb;
	};

	class cpplog
	{
		public:
			cpplog(std::ostream &outStream, int logMask) :
				stream(outStream), levelmask(logMask)
			{
			}

			cpplog(std::ostream &outStream) :
				cpplog(outStream, LOG_ERROR)
			{ }

			cpplog(int logMask) :
				cpplog(std::cout, logMask)
			{ }

			cpplog() :
				cpplog(std::cout, LOG_ERROR)
			{ }

			~cpplog()
			{ }

			void SetLogMask(int logMask)
			{
				levelmask = logMask;
			}

			std::ostream &getStream(loglevel_t lv)
			{
				if (lv & levelmask)
					return stream;
				else
					return null_stream;
			}

		private:
			std::ostream &stream;
			NullStream null_stream;
			int levelmask;
	};
}

#define LOG_LEVEL(level, logger)	logger.getStream(level)

#define ELOG(logger)	LOG_LEVEL(LOG_ERROR, logger)
#define WLOG(logger)	LOG_LEVEL(LOG_WARN, logger)
#define DLOG(logger)	LOG_LEVEL(LOG_DEBUG, logger)
#define ILOG(logger)	LOG_LEVEL(LOG_INFO, logger)
#define VLOG(logger)	LOG_LEVEL(LOG_VERBOSE, logger)

#endif
