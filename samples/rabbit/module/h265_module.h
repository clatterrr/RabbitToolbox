#ifndef RABBIT_H265_MODULE_H_
#define RABBIT_H265_MODULE_H_

#include <rabbit_model.h>
#include <cstdint>

namespace rabbit
{
	enum VideoQuality
	{
		Low,
		Medium,
		High
	};

	class H265Module final : public RabbitModule
	{
	public:
		H265Module() noexcept;
		virtual ~H265Module() noexcept;

		void setVideoQuality(VideoQuality quality);

	public:
		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		H265Module(const H265Module&) = delete;
		H265Module& operator=(const H265Module&) = delete;

	public:
		bool enable;

		VideoQuality quality;

		double crf;

		std::uint32_t frame_type;
		std::uint32_t encode_speed;
	};
}

#endif