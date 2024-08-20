#pragma once

#include <base/info/media_track.h>
#include <base/mediarouter/media_buffer.h>
#include <base/ovlibrary/ovlibrary.h>

extern "C"
{
#include <libavformat/avformat.h>
};

namespace ffmpeg
{
	class Writer
	{
	public:
		enum TimestampMode
		{
			TIMESTAMP_STARTZERO_MODE = 0,
			TIMESTAMP_PASSTHROUGH_MODE = 1
		};

		enum WriterState
		{
			WriterStateNone = 0,
			WriterStateConnecting = 1,
			WriterStateConnected = 2,
			WriterStateError = 3,
			WriterStateClosed = 4
		};

	public:
		static std::shared_ptr<Writer> Create();

		Writer();
		~Writer();

		static int InterruptCallback(void *opaque);

		// format is muxer(or container)
		// 	- RTMP : flv
		// 	- MPEGTS : mpegts, mp4
		//  - SRT : mpegts
		bool SetUrl(const ov::String url, const ov::String format = nullptr);
		ov::String GetUrl();

		bool Start();
		bool Stop();

		bool AddTrack(std::shared_ptr<MediaTrack> media_track);
		bool SendPacket(std::shared_ptr<MediaPacket> packet);
		std::chrono::high_resolution_clock::time_point GetLastPacketSentTime();

		void SetTimestampMode(TimestampMode mode);
		TimestampMode GetTimestampMode();

		void SetState(WriterState state);
		WriterState GetState();
		
	private:
		WriterState _state;

		ov::String _url;
		ov::String _format;

		int64_t _start_time = -1LL;
		TimestampMode _timestamp_mode = TIMESTAMP_STARTZERO_MODE;

		bool _need_to_flush = false;
		bool _need_to_close = false;

		// MediaTrackId -> AVStream, MediaTrack
		std::map<int32_t, std::pair<AVStream*, std::shared_ptr<MediaTrack>>> _track_map;

		AVFormatContext* _av_format = nullptr;

		AVIOInterruptCB _interrupt_cb;
		std::chrono::high_resolution_clock::time_point _last_packet_sent_time;
		int32_t _connection_timeout = 5000;	// 5s
		int32_t _send_timeout 		= 1000;	// 1s

		std::mutex _lock;
	};
}  // namespace ffmpeg