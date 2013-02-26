/* soapWSDualHttpBinding_USCOREICalculatorDuplexProxy.cpp
   Generated by gSOAP 2.8.12 from calculator.h

Copyright(C) 2000-2012, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "soapWSDualHttpBinding_USCOREICalculatorDuplexProxy.h"

WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy()
{	this->soap = soap_new();
	this->own = true;
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy(struct soap *_soap)
{	this->soap = _soap;
	this->own = false;
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(_soap->imode, _soap->omode);
}

WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy(const char *url)
{	this->soap = soap_new();
	this->own = true;
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
	soap_endpoint = url;
}

WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy(soap_mode iomode)
{	this->soap = soap_new();
	this->own = true;
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(iomode, iomode);
}

WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy(const char *url, soap_mode iomode)
{	this->soap = soap_new();
	this->own = true;
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(iomode, iomode);
	soap_endpoint = url;
}

WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy(soap_mode imode, soap_mode omode)
{	this->soap = soap_new();
	this->own = true;
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(imode, omode);
}

WSDualHttpBinding_USCOREICalculatorDuplexProxy::~WSDualHttpBinding_USCOREICalculatorDuplexProxy()
{	if (this->own)
		soap_free(this->soap);
}

void WSDualHttpBinding_USCOREICalculatorDuplexProxy::WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(soap_mode imode, soap_mode omode)
{	soap_imode(this->soap, imode);
	soap_omode(this->soap, omode);
	soap_endpoint = NULL;
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"tempuri", "http://tempuri.org/", NULL, NULL},
	{"mssadh", "http://Microsoft.Samples.DualHttp", NULL, NULL},
	{"chan", "http://schemas.microsoft.com/ws/2005/02/duplex", NULL, NULL},
	{"wsa5", "http://www.w3.org/2005/08/addressing", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL},
	{"wsrm", "http://schemas.xmlsoap.org/ws/2005/02/rm", "http://docs.oasis-open.org/ws-rx/wsrm/200702", NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this->soap, namespaces);
}

void WSDualHttpBinding_USCOREICalculatorDuplexProxy::destroy()
{	soap_destroy(this->soap);
	soap_end(this->soap);
}

void WSDualHttpBinding_USCOREICalculatorDuplexProxy::reset()
{	destroy();
	soap_done(this->soap);
	soap_init(this->soap);
	WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

void WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_noheader()
{	this->soap->header = NULL;
}

void WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_header(char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance, struct wsrm__SequenceType *wsrm__Sequence, int __sizeAckRequested, struct wsrm__AckRequestedType *wsrm__AckRequested, int __sizeSequenceAcknowledgement, struct _wsrm__SequenceAcknowledgement *wsrm__SequenceAcknowledgement, struct wsrm__SequenceFaultType *wsrm__SequenceFault)
{	::soap_header(this->soap);
	this->soap->header->wsa5__MessageID = wsa5__MessageID;
	this->soap->header->wsa5__RelatesTo = wsa5__RelatesTo;
	this->soap->header->wsa5__From = wsa5__From;
	this->soap->header->wsa5__ReplyTo = wsa5__ReplyTo;
	this->soap->header->wsa5__FaultTo = wsa5__FaultTo;
	this->soap->header->wsa5__To = wsa5__To;
	this->soap->header->wsa5__Action = wsa5__Action;
	this->soap->header->chan__ChannelInstance = chan__ChannelInstance;
	this->soap->header->wsrm__Sequence = wsrm__Sequence;
	this->soap->header->__sizeAckRequested = __sizeAckRequested;
	this->soap->header->wsrm__AckRequested = wsrm__AckRequested;
	this->soap->header->__sizeSequenceAcknowledgement = __sizeSequenceAcknowledgement;
	this->soap->header->wsrm__SequenceAcknowledgement = wsrm__SequenceAcknowledgement;
	this->soap->header->wsrm__SequenceFault = wsrm__SequenceFault;
}

const SOAP_ENV__Header *WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_header()
{	return this->soap->header;
}

const SOAP_ENV__Fault *WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_fault()
{	return this->soap->fault;
}

const char *WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_fault_string()
{	return *soap_faultstring(this->soap);
}

const char *WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_fault_detail()
{	return *soap_faultdetail(this->soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_close_socket()
{	return soap_closesock(this->soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_force_close_socket()
{	return soap_force_closesock(this->soap);
}

void WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_print_fault(FILE *fd)
{	::soap_print_fault(this->soap, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this->soap, os);
}
#endif

char *WSDualHttpBinding_USCOREICalculatorDuplexProxy::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this->soap, buf, len);
}
#endif

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_Clear(const char *endpoint, const char *soap_action, _mssadh__Clear *mssadh__Clear)
{	struct soap *soap = this->soap;
	struct __tempuri__Clear soap_tmp___tempuri__Clear;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/Clear";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__Clear.mssadh__Clear = mssadh__Clear;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__Clear(soap, &soap_tmp___tempuri__Clear);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__Clear(soap, &soap_tmp___tempuri__Clear, "-tempuri:Clear", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__Clear(soap, &soap_tmp___tempuri__Clear, "-tempuri:Clear", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_Clear(struct __tempuri__Clear& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__Clear *_param_1 = &tmp;
	soap_default___tempuri__Clear(soap, _param_1);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__Clear(soap, _param_1, "-tempuri:Clear", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_AddTo(const char *endpoint, const char *soap_action, _mssadh__AddTo *mssadh__AddTo)
{	struct soap *soap = this->soap;
	struct __tempuri__AddTo soap_tmp___tempuri__AddTo;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/AddTo";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__AddTo.mssadh__AddTo = mssadh__AddTo;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__AddTo(soap, &soap_tmp___tempuri__AddTo);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__AddTo(soap, &soap_tmp___tempuri__AddTo, "-tempuri:AddTo", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__AddTo(soap, &soap_tmp___tempuri__AddTo, "-tempuri:AddTo", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_AddTo(struct __tempuri__AddTo& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__AddTo *_param_2 = &tmp;
	soap_default___tempuri__AddTo(soap, _param_2);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__AddTo(soap, _param_2, "-tempuri:AddTo", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_SubtractFrom(const char *endpoint, const char *soap_action, _mssadh__SubtractFrom *mssadh__SubtractFrom)
{	struct soap *soap = this->soap;
	struct __tempuri__SubtractFrom soap_tmp___tempuri__SubtractFrom;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/SubtractFrom";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__SubtractFrom.mssadh__SubtractFrom = mssadh__SubtractFrom;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__SubtractFrom(soap, &soap_tmp___tempuri__SubtractFrom);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__SubtractFrom(soap, &soap_tmp___tempuri__SubtractFrom, "-tempuri:SubtractFrom", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__SubtractFrom(soap, &soap_tmp___tempuri__SubtractFrom, "-tempuri:SubtractFrom", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_SubtractFrom(struct __tempuri__SubtractFrom& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__SubtractFrom *_param_3 = &tmp;
	soap_default___tempuri__SubtractFrom(soap, _param_3);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__SubtractFrom(soap, _param_3, "-tempuri:SubtractFrom", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_MultiplyBy(const char *endpoint, const char *soap_action, _mssadh__MultiplyBy *mssadh__MultiplyBy)
{	struct soap *soap = this->soap;
	struct __tempuri__MultiplyBy soap_tmp___tempuri__MultiplyBy;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/MultiplyBy";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__MultiplyBy.mssadh__MultiplyBy = mssadh__MultiplyBy;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__MultiplyBy(soap, &soap_tmp___tempuri__MultiplyBy);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__MultiplyBy(soap, &soap_tmp___tempuri__MultiplyBy, "-tempuri:MultiplyBy", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__MultiplyBy(soap, &soap_tmp___tempuri__MultiplyBy, "-tempuri:MultiplyBy", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_MultiplyBy(struct __tempuri__MultiplyBy& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__MultiplyBy *_param_4 = &tmp;
	soap_default___tempuri__MultiplyBy(soap, _param_4);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__MultiplyBy(soap, _param_4, "-tempuri:MultiplyBy", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_DivideBy(const char *endpoint, const char *soap_action, _mssadh__DivideBy *mssadh__DivideBy)
{	struct soap *soap = this->soap;
	struct __tempuri__DivideBy soap_tmp___tempuri__DivideBy;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/DivideBy";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__DivideBy.mssadh__DivideBy = mssadh__DivideBy;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__DivideBy(soap, &soap_tmp___tempuri__DivideBy);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__DivideBy(soap, &soap_tmp___tempuri__DivideBy, "-tempuri:DivideBy", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__DivideBy(soap, &soap_tmp___tempuri__DivideBy, "-tempuri:DivideBy", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_DivideBy(struct __tempuri__DivideBy& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__DivideBy *_param_5 = &tmp;
	soap_default___tempuri__DivideBy(soap, _param_5);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__DivideBy(soap, _param_5, "-tempuri:DivideBy", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_Result(const char *endpoint, const char *soap_action, _mssadh__Result *mssadh__Result)
{	struct soap *soap = this->soap;
	struct __tempuri__Result soap_tmp___tempuri__Result;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/Result";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__Result.mssadh__Result = mssadh__Result;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__Result(soap, &soap_tmp___tempuri__Result);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__Result(soap, &soap_tmp___tempuri__Result, "-tempuri:Result", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__Result(soap, &soap_tmp___tempuri__Result, "-tempuri:Result", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_Result(struct __tempuri__Result& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__Result *_param_6 = &tmp;
	soap_default___tempuri__Result(soap, _param_6);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__Result(soap, _param_6, "-tempuri:Result", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::send_Equation(const char *endpoint, const char *soap_action, _mssadh__Equation *mssadh__Equation)
{	struct soap *soap = this->soap;
	struct __tempuri__Equation soap_tmp___tempuri__Equation;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.2:8000/ServiceModelSamples/service";
	if (soap_action == NULL)
		soap_action = "http://Microsoft.Samples.DualHttp/ICalculatorDuplex/Equation";
	soap->encodingStyle = NULL;
	soap_tmp___tempuri__Equation.mssadh__Equation = mssadh__Equation;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___tempuri__Equation(soap, &soap_tmp___tempuri__Equation);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tempuri__Equation(soap, &soap_tmp___tempuri__Equation, "-tempuri:Equation", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tempuri__Equation(soap, &soap_tmp___tempuri__Equation, "-tempuri:Equation", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int WSDualHttpBinding_USCOREICalculatorDuplexProxy::recv_Equation(struct __tempuri__Equation& tmp)
{	struct soap *soap = this->soap;

	struct __tempuri__Equation *_param_7 = &tmp;
	soap_default___tempuri__Equation(soap, _param_7);
	soap_begin(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get___tempuri__Equation(soap, _param_7, "-tempuri:Equation", NULL);
	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
		soap->error = SOAP_NO_METHOD;
	if (soap->error
	 || soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}
/* End of client proxy code */
