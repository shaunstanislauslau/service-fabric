// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Management
{
    namespace FaultAnalysisService
    {
        class FaultAnalysisServiceAgent;

        class ClientRequestAsyncOperation
            : public Common::TimedAsyncOperation
        {
        public:

            ClientRequestAsyncOperation(
                __in FaultAnalysisServiceAgent &,
                Common::ActivityId const &,
                Common::TimeSpan const,
                Common::AsyncCallback const &,
                Common::AsyncOperationSPtr const &);

            ClientRequestAsyncOperation(
                __in FaultAnalysisServiceAgent &,
                Transport::MessageUPtr &&,
                Transport::IpcReceiverContextUPtr &&,
                Common::TimeSpan const,
                Common::AsyncCallback const &,
                Common::AsyncOperationSPtr const &);

            ClientRequestAsyncOperation(
                __in FaultAnalysisServiceAgent &,
                Common::ErrorCode const &,
                Transport::MessageUPtr &&,
                Transport::IpcReceiverContextUPtr &&,
                Common::TimeSpan const,
                Common::AsyncCallback const &,
                Common::AsyncOperationSPtr const &);

            ~ClientRequestAsyncOperation();

            static Common::ErrorCode End(
                Common::AsyncOperationSPtr const & asyncOperation,
                __out Transport::MessageUPtr & reply,
                __out Transport::IpcReceiverContextUPtr & receiverContext);

            __declspec(property(get = get_Timeout)) Common::TimeSpan Timeout;
            __declspec(property(get = get_RequestMessageId)) Transport::MessageId const & RequestMessageId;

            Common::TimeSpan get_Timeout() const { return this->OriginalTimeout; }
            Transport::MessageId get_RequestMessageId() const { return request_->MessageId; }

            bool IsLocalRetry();

            static bool IsRetryable(Common::ErrorCode const &);

        protected:
            __declspec(property(get = get_HasRequest)) bool HasRequest;
            __declspec(property(get = get_Agent)) FaultAnalysisServiceAgent & Agent;
            __declspec(property(get = get_Request)) Transport::Message & Request;
            __declspec(property(get = get_ReceiverContext)) Transport::IpcReceiverContext const & ReceiverContext;

            bool get_HasRequest() const { return request_ != nullptr; }
            FaultAnalysisServiceAgent & get_Agent() const { return owner_; }
            Transport::Message & get_Request() const { return *request_; }
            Transport::IpcReceiverContext const & get_ReceiverContext() const { return *receiverContext_; }

            void OnStart(Common::AsyncOperationSPtr const &);
            void OnTimeout(Common::AsyncOperationSPtr const &);
            void OnCancel();
            void OnCompleted();

            void SetReply(Transport::MessageUPtr &&);

            virtual Common::AsyncOperationSPtr BeginAcceptRequest(
                std::shared_ptr<ClientRequestAsyncOperation> &&,
                Common::TimeSpan const,
                Common::AsyncCallback const &,
                Common::AsyncOperationSPtr const &);
            virtual Common::ErrorCode EndAcceptRequest(Common::AsyncOperationSPtr const &);

        private:
            void HandleRequest(Common::AsyncOperationSPtr const &);
            void OnAcceptRequestComplete(Common::AsyncOperationSPtr const &, bool expectedCompletedSynchronously);
            void CancelRetryTimer();

            FaultAnalysisServiceAgent & owner_;
            Common::ErrorCodeValue::Enum error_;
            Transport::MessageUPtr request_;
            Transport::MessageUPtr reply_;
            Transport::IpcReceiverContextUPtr receiverContext_;
            Common::TimerSPtr retryTimer_;
            Common::ExclusiveLock timerLock_;
            bool isLocalRetry_;
        };

        typedef std::shared_ptr<ClientRequestAsyncOperation> ClientRequestSPtr;
    }
}

