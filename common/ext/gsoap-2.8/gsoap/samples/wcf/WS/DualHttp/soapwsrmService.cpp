/* soapwsrmService.cpp
   Generated by gSOAP 2.8.12 from calculator.h

Copyright(C) 2000-2012, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "soapwsrmService.h"

wsrmService::wsrmService()
{	this->soap = soap_new();
	this->own = true;
	wsrmService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

wsrmService::wsrmService(struct soap *_soap)
{	this->soap = _soap;
	this->own = false;
	wsrmService_init(_soap->imode, _soap->omode);
}

wsrmService::wsrmService(soap_mode iomode)
{	this->soap = soap_new();
	this->own = true;
	wsrmService_init(iomode, iomode);
}

wsrmService::wsrmService(soap_mode imode, soap_mode omode)
{	this->soap = soap_new();
	this->own = true;
	wsrmService_init(imode, omode);
}

wsrmService::~wsrmService()
{	if (this->own)
		soap_free(this->soap);
}

void wsrmService::wsrmService_init(soap_mode imode, soap_mode omode)
{	soap_imode(this->soap, imode);
	soap_omode(this->soap, omode);
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
};

void wsrmService::destroy()
{	soap_destroy(this->soap);
	soap_end(this->soap);
}

void wsrmService::reset()
{	destroy();
	soap_done(this->soap);
	soap_init(this->soap);
	wsrmService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

#ifndef WITH_PURE_VIRTUAL
wsrmService *wsrmService::copy()
{	wsrmService *dup = SOAP_NEW_COPY(wsrmService);
	if (dup)
		soap_copy_context(dup->soap, this->soap);
	return dup;
}
#endif

int wsrmService::soap_close_socket()
{	return soap_closesock(this->soap);
}

int wsrmService::soap_force_close_socket()
{	return soap_force_closesock(this->soap);
}

int wsrmService::soap_senderfault(const char *string, const char *detailXML)
{	return ::soap_sender_fault(this->soap, string, detailXML);
}

int wsrmService::soap_senderfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_sender_fault_subcode(this->soap, subcodeQName, string, detailXML);
}

int wsrmService::soap_receiverfault(const char *string, const char *detailXML)
{	return ::soap_receiver_fault(this->soap, string, detailXML);
}

int wsrmService::soap_receiverfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_receiver_fault_subcode(this->soap, subcodeQName, string, detailXML);
}

void wsrmService::soap_print_fault(FILE *fd)
{	::soap_print_fault(this->soap, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void wsrmService::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this->soap, os);
}
#endif

char *wsrmService::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this->soap, buf, len);
}
#endif

void wsrmService::soap_noheader()
{	this->soap->header = NULL;
}

void wsrmService::soap_header(char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance, struct wsrm__SequenceType *wsrm__Sequence, int __sizeAckRequested, struct wsrm__AckRequestedType *wsrm__AckRequested, int __sizeSequenceAcknowledgement, struct _wsrm__SequenceAcknowledgement *wsrm__SequenceAcknowledgement, struct wsrm__SequenceFaultType *wsrm__SequenceFault)
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

const SOAP_ENV__Header *wsrmService::soap_header()
{	return this->soap->header;
}

int wsrmService::run(int port)
{	if (soap_valid_socket(this->soap->master) || soap_valid_socket(bind(NULL, port, 100)))
	{	for (;;)
		{	if (!soap_valid_socket(accept()) || serve())
				return this->soap->error;
			soap_destroy(this->soap);
			soap_end(this->soap);
		}
	}
	else
		return this->soap->error;
	return SOAP_OK;
}

SOAP_SOCKET wsrmService::bind(const char *host, int port, int backlog)
{	return soap_bind(this->soap, host, port, backlog);
}

SOAP_SOCKET wsrmService::accept()
{	return soap_accept(this->soap);
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int wsrmService::ssl_accept()
{	return soap_ssl_accept(this->soap);
}
#endif

int wsrmService::serve()
{
#ifndef WITH_FASTCGI
	unsigned int k = this->soap->max_keep_alive;
#endif
	do
	{

#ifndef WITH_FASTCGI
		if (this->soap->max_keep_alive > 0 && !--k)
			this->soap->keep_alive = 0;
#endif

		if (soap_begin_serve(this->soap))
		{	if (this->soap->error >= SOAP_STOP)
				continue;
			return this->soap->error;
		}
		if (dispatch() || (this->soap->fserveloop && this->soap->fserveloop(this->soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(this->soap);
#else
			return soap_send_fault(this->soap);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(this->soap);
		soap_end(this->soap);
	} while (1);
#else
	} while (this->soap->keep_alive);
#endif
	return SOAP_OK;
}

static int serve___wsrm__CreateSequence(wsrmService*);
static int serve___wsrm__CloseSequence(wsrmService*);
static int serve___wsrm__TerminateSequence(wsrmService*);
static int serve___wsrm__CreateSequenceResponse(wsrmService*);
static int serve___wsrm__CloseSequenceResponse(wsrmService*);
static int serve___wsrm__TerminateSequenceResponse(wsrmService*);
static int serve___wsrm__SequenceAcknowledgement(wsrmService*);
static int serve___wsrm__AckRequested(wsrmService*);
static int serve___wsrm__LastMessage(wsrmService*);

int wsrmService::dispatch()
{	wsrmService_init(this->soap->imode, this->soap->omode);
	soap_peek_element(this->soap);
	if ((!this->soap->action && !soap_match_tag(this->soap, this->soap->tag, "wsrm:CreateSequence")) || (this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/CreateSequence")))
		return serve___wsrm__CreateSequence(this);
	if ((!this->soap->action && !soap_match_tag(this->soap, this->soap->tag, "wsrm:CloseSequence")) || (this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/CloseSequence")))
		return serve___wsrm__CloseSequence(this);
	if ((!this->soap->action && !soap_match_tag(this->soap, this->soap->tag, "wsrm:TerminateSequence")) || (this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/TerminateSequence")))
		return serve___wsrm__TerminateSequence(this);
	if ((!this->soap->action && !soap_match_tag(this->soap, this->soap->tag, "wsrm:CreateSequenceResponse")) || (this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/CreateSequenceResponse")))
		return serve___wsrm__CreateSequenceResponse(this);
	if ((!this->soap->action && !soap_match_tag(this->soap, this->soap->tag, "wsrm:CloseSequenceResponse")) || (this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/CloseSequenceResponse")))
		return serve___wsrm__CloseSequenceResponse(this);
	if ((!this->soap->action && !soap_match_tag(this->soap, this->soap->tag, "wsrm:TerminateSequenceResponse")) || (this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/TerminateSequenceResponse")))
		return serve___wsrm__TerminateSequenceResponse(this);
	if ((this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/SequenceAcknowledgement")))
		return serve___wsrm__SequenceAcknowledgement(this);
	if ((this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/AckRequested")))
		return serve___wsrm__AckRequested(this);
	if ((this->soap->action && !strcmp(this->soap->action, "http://schemas.xmlsoap.org/ws/2005/02/rm/LastMessage")))
		return serve___wsrm__LastMessage(this);
	return serve___wsrm__LastMessage(this);
}

static int serve___wsrm__CreateSequence(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__CreateSequence soap_tmp___wsrm__CreateSequence;
	struct wsrm__CreateSequenceResponseType wsrm__CreateSequenceResponse;
	soap_default_wsrm__CreateSequenceResponseType(soap, &wsrm__CreateSequenceResponse);
	soap_default___wsrm__CreateSequence(soap, &soap_tmp___wsrm__CreateSequence);
	soap->encodingStyle = NULL;
	if (!soap_get___wsrm__CreateSequence(soap, &soap_tmp___wsrm__CreateSequence, "-wsrm:CreateSequence", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->CreateSequence(soap_tmp___wsrm__CreateSequence.wsrm__CreateSequence, &wsrm__CreateSequenceResponse);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_wsrm__CreateSequenceResponseType(soap, &wsrm__CreateSequenceResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_wsrm__CreateSequenceResponseType(soap, &wsrm__CreateSequenceResponse, "wsrm:CreateSequenceResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_wsrm__CreateSequenceResponseType(soap, &wsrm__CreateSequenceResponse, "wsrm:CreateSequenceResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__CloseSequence(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__CloseSequence soap_tmp___wsrm__CloseSequence;
	struct wsrm__CloseSequenceResponseType wsrm__CloseSequenceResponse;
	soap_default_wsrm__CloseSequenceResponseType(soap, &wsrm__CloseSequenceResponse);
	soap_default___wsrm__CloseSequence(soap, &soap_tmp___wsrm__CloseSequence);
	soap->encodingStyle = NULL;
	if (!soap_get___wsrm__CloseSequence(soap, &soap_tmp___wsrm__CloseSequence, "-wsrm:CloseSequence", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->CloseSequence(soap_tmp___wsrm__CloseSequence.wsrm__CloseSequence, &wsrm__CloseSequenceResponse);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_wsrm__CloseSequenceResponseType(soap, &wsrm__CloseSequenceResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_wsrm__CloseSequenceResponseType(soap, &wsrm__CloseSequenceResponse, "wsrm:CloseSequenceResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_wsrm__CloseSequenceResponseType(soap, &wsrm__CloseSequenceResponse, "wsrm:CloseSequenceResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__TerminateSequence(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__TerminateSequence soap_tmp___wsrm__TerminateSequence;
	struct wsrm__TerminateSequenceResponseType wsrm__TerminateSequence_;
	soap_default_wsrm__TerminateSequenceResponseType(soap, &wsrm__TerminateSequence_);
	soap_default___wsrm__TerminateSequence(soap, &soap_tmp___wsrm__TerminateSequence);
	soap->encodingStyle = NULL;
	if (!soap_get___wsrm__TerminateSequence(soap, &soap_tmp___wsrm__TerminateSequence, "-wsrm:TerminateSequence", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->TerminateSequence(soap_tmp___wsrm__TerminateSequence.wsrm__TerminateSequence, &wsrm__TerminateSequence_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_wsrm__TerminateSequenceResponseType(soap, &wsrm__TerminateSequence_);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_wsrm__TerminateSequenceResponseType(soap, &wsrm__TerminateSequence_, "wsrm:TerminateSequence", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_wsrm__TerminateSequenceResponseType(soap, &wsrm__TerminateSequence_, "wsrm:TerminateSequence", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__CreateSequenceResponse(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__CreateSequenceResponse soap_tmp___wsrm__CreateSequenceResponse;
	soap_default___wsrm__CreateSequenceResponse(soap, &soap_tmp___wsrm__CreateSequenceResponse);
	soap->encodingStyle = NULL;
	if (!soap_get___wsrm__CreateSequenceResponse(soap, &soap_tmp___wsrm__CreateSequenceResponse, "-wsrm:CreateSequenceResponse", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->CreateSequenceResponse(soap_tmp___wsrm__CreateSequenceResponse.wsrm__CreateSequenceResponse);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__CloseSequenceResponse(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__CloseSequenceResponse soap_tmp___wsrm__CloseSequenceResponse;
	soap_default___wsrm__CloseSequenceResponse(soap, &soap_tmp___wsrm__CloseSequenceResponse);
	soap->encodingStyle = NULL;
	if (!soap_get___wsrm__CloseSequenceResponse(soap, &soap_tmp___wsrm__CloseSequenceResponse, "-wsrm:CloseSequenceResponse", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->CloseSequenceResponse(soap_tmp___wsrm__CloseSequenceResponse.wsrm__CloseSequenceResponse);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__TerminateSequenceResponse(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__TerminateSequenceResponse soap_tmp___wsrm__TerminateSequenceResponse;
	soap_default___wsrm__TerminateSequenceResponse(soap, &soap_tmp___wsrm__TerminateSequenceResponse);
	soap->encodingStyle = NULL;
	if (!soap_get___wsrm__TerminateSequenceResponse(soap, &soap_tmp___wsrm__TerminateSequenceResponse, "-wsrm:TerminateSequenceResponse", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->TerminateSequenceResponse(soap_tmp___wsrm__TerminateSequenceResponse.wsrm__TerminateSequenceResponse);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__SequenceAcknowledgement(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__SequenceAcknowledgement soap_tmp___wsrm__SequenceAcknowledgement;
	soap_default___wsrm__SequenceAcknowledgement(soap, &soap_tmp___wsrm__SequenceAcknowledgement);
	soap->encodingStyle = NULL;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->SequenceAcknowledgement();
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__AckRequested(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__AckRequested soap_tmp___wsrm__AckRequested;
	soap_default___wsrm__AckRequested(soap, &soap_tmp___wsrm__AckRequested);
	soap->encodingStyle = NULL;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->AckRequested();
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsrm__LastMessage(wsrmService *service)
{	struct soap *soap = service->soap;
	struct __wsrm__LastMessage soap_tmp___wsrm__LastMessage;
	soap_default___wsrm__LastMessage(soap, &soap_tmp___wsrm__LastMessage);
	soap->encodingStyle = NULL;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->LastMessage();
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}
/* End of server object code */
