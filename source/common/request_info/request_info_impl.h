#pragma once

#include <chrono>
#include <cstdint>

#include "envoy/request_info/request_info.h"

#include "common/common/assert.h"

namespace Envoy {
namespace RequestInfo {

struct RequestInfoImpl : public RequestInfo {
  RequestInfoImpl()
      : start_time_(std::chrono::system_clock::now()),
        start_time_monotonic_(std::chrono::steady_clock::now()) {}

  RequestInfoImpl(Http::Protocol protocol) : RequestInfoImpl() { protocol_ = protocol; }

  SystemTime startTime() const override { return start_time_; }

  MonotonicTime startTimeMonotonic() const override { return start_time_monotonic_; }

  absl::optional<std::chrono::nanoseconds> duration(absl::optional<MonotonicTime> time) const {
    if (!time) {
      return {};
    }

    return std::chrono::duration_cast<std::chrono::nanoseconds>(time.value() -
                                                                start_time_monotonic_);
  }

  absl::optional<std::chrono::nanoseconds> lastDownstreamRxByteReceived() const override {
    return duration(last_downstream_rx_byte_received);
  }

  void onLastDownstreamRxByteReceived() override {
    ASSERT(!last_downstream_rx_byte_received);
    last_downstream_rx_byte_received = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> firstUpstreamTxByteSent() const override {
    return duration(first_upstream_tx_byte_sent_);
  }

  void onFirstUpstreamTxByteSent() override {
    ASSERT(!first_upstream_tx_byte_sent_);
    first_upstream_tx_byte_sent_ = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> lastUpstreamTxByteSent() const override {
    return duration(last_upstream_tx_byte_sent_);
  }

  void onLastUpstreamTxByteSent() override {
    ASSERT(!last_upstream_tx_byte_sent_);
    last_upstream_tx_byte_sent_ = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> firstUpstreamRxByteReceived() const override {
    return duration(first_upstream_rx_byte_received_);
  }

  void onFirstUpstreamRxByteReceived() override {
    ASSERT(!first_upstream_rx_byte_received_);
    first_upstream_rx_byte_received_ = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> lastUpstreamRxByteReceived() const override {
    return duration(last_upstream_rx_byte_received_);
  }

  void onLastUpstreamRxByteReceived() override {
    ASSERT(!last_upstream_rx_byte_received_);
    last_upstream_rx_byte_received_ = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> firstDownstreamTxByteSent() const override {
    return duration(first_downstream_tx_byte_sent_);
  }

  void onFirstDownstreamTxByteSent() override {
    ASSERT(!first_downstream_tx_byte_sent_);
    first_downstream_tx_byte_sent_ = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> lastDownstreamTxByteSent() const override {
    return duration(last_downstream_tx_byte_sent_);
  }

  void onLastDownstreamTxByteSent() override {
    ASSERT(!last_downstream_tx_byte_sent_);
    last_downstream_tx_byte_sent_ = std::chrono::steady_clock::now();
  }

  absl::optional<std::chrono::nanoseconds> requestComplete() const override {
    return duration(final_time_);
  }

  void onRequestComplete() override {
    ASSERT(!final_time_);
    final_time_ = std::chrono::steady_clock::now();
  }

  void resetUpstreamTimings() override {
    first_upstream_tx_byte_sent_ = absl::optional<MonotonicTime>{};
    last_upstream_tx_byte_sent_ = absl::optional<MonotonicTime>{};
    first_upstream_rx_byte_received_ = absl::optional<MonotonicTime>{};
    last_upstream_rx_byte_received_ = absl::optional<MonotonicTime>{};
  }

  uint64_t bytesReceived() const override { return bytes_received_; }

  absl::optional<Http::Protocol> protocol() const override { return protocol_; }

  void protocol(Http::Protocol protocol) override { protocol_ = protocol; }

  absl::optional<uint32_t> responseCode() const override { return response_code_; }

  uint64_t bytesSent() const override { return bytes_sent_; }

  void setResponseFlag(ResponseFlag response_flag) override { response_flags_ |= response_flag; }

  bool getResponseFlag(ResponseFlag flag) const override { return response_flags_ & flag; }

  void onUpstreamHostSelected(Upstream::HostDescriptionConstSharedPtr host) override {
    upstream_host_ = host;
  }

  Upstream::HostDescriptionConstSharedPtr upstreamHost() const override { return upstream_host_; }

  const Network::Address::InstanceConstSharedPtr& upstreamLocalAddress() const override {
    return upstream_local_address_;
  }

  bool healthCheck() const override { return hc_request_; }

  void healthCheck(bool is_hc) override { hc_request_ = is_hc; }

  const Network::Address::InstanceConstSharedPtr& downstreamLocalAddress() const override {
    return downstream_local_address_;
  }

  const Network::Address::InstanceConstSharedPtr& downstreamRemoteAddress() const override {
    return downstream_remote_address_;
  }

  const Router::RouteEntry* routeEntry() const override { return route_entry_; }

  const envoy::api::v2::core::Metadata& dynamicMetadata() const override { return metadata_; };

  void setDynamicMetadata(const std::string& name, const ProtobufWkt::Struct& value) override {
    (*metadata_.mutable_filter_metadata())[name].MergeFrom(value);
  };

  const SystemTime start_time_;
  const MonotonicTime start_time_monotonic_;

  absl::optional<MonotonicTime> last_downstream_rx_byte_received;
  absl::optional<MonotonicTime> first_upstream_tx_byte_sent_;
  absl::optional<MonotonicTime> last_upstream_tx_byte_sent_;
  absl::optional<MonotonicTime> first_upstream_rx_byte_received_;
  absl::optional<MonotonicTime> last_upstream_rx_byte_received_;
  absl::optional<MonotonicTime> first_downstream_tx_byte_sent_;
  absl::optional<MonotonicTime> last_downstream_tx_byte_sent_;
  absl::optional<MonotonicTime> final_time_;

  absl::optional<Http::Protocol> protocol_;
  uint64_t bytes_received_{};
  absl::optional<uint32_t> response_code_;
  uint64_t bytes_sent_{};
  uint64_t response_flags_{};
  Upstream::HostDescriptionConstSharedPtr upstream_host_{};
  Network::Address::InstanceConstSharedPtr upstream_local_address_;
  bool hc_request_{};
  Network::Address::InstanceConstSharedPtr downstream_local_address_;
  Network::Address::InstanceConstSharedPtr downstream_remote_address_;
  const Router::RouteEntry* route_entry_{};
  envoy::api::v2::core::Metadata metadata_{};
};

} // namespace RequestInfo
} // namespace Envoy
