/*
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "dds/DCPS/WaitSet.h"

#include "DataReaderListenerImpl.h"
#include "MessengerTypeSupportC.h"
#include "MessengerTypeSupportImpl.h"

#include <iostream>

DataReaderListenerImpl::~DataReaderListenerImpl()
{
  ACE_DEBUG((LM_DEBUG, "(%P|%t) DataReader %C is done\n", id_.c_str()));
  if (expected_samples_ && received_samples_ != expected_samples_) {
    ACE_ERROR((LM_ERROR, "ERROR: DataReader %C expected %d but received %d\n",
               id_.c_str(), expected_samples_, received_samples_));
  } else if (expected_samples_) {
    ACE_DEBUG((LM_DEBUG, "(%P|%t) DataReader %C Expected number of samples received\n", id_.c_str()));
  }
}

void
DataReaderListenerImpl::on_requested_deadline_missed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_requested_incompatible_qos(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_rejected(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleRejectedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_liveliness_changed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
  DDS::DomainId_t domain = reader->get_subscriber()->get_participant()->get_domain_id();

  Messenger::MessageDataReader_var reader_i =
    Messenger::MessageDataReader::_narrow(reader);

  if (!reader_i) {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" _narrow failed!\n")));
    ACE_OS::exit(EXIT_FAILURE);
  }

  Messenger::Message message;
  DDS::SampleInfo info;

  DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

  if (error == DDS::RETCODE_OK) {
    if (info.valid_data) {
      ACE_DEBUG((LM_DEBUG, "(%P|%t) Domain: %d. %C received message %d from %C.\n", domain, id_.c_str(), message.count, std::string(message.from).c_str()));

      if (++received_samples_ == expected_samples_) {
        ACE_DEBUG((LM_DEBUG, "(%P|%t) DataReader %C has received expected number of samples\n", id_.c_str()));
        done_callback_();
      }
    }

  } else {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" take_next_sample failed!\n")));
  }
}

void
DataReaderListenerImpl::on_subscription_matched(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_lost(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleLostStatus& /*status*/)
{
}
