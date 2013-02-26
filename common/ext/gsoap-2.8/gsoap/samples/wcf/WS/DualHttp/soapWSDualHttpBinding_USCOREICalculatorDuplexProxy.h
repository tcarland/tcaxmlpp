/* soapWSDualHttpBinding_USCOREICalculatorDuplexProxy.h
   Generated by gSOAP 2.8.12 from calculator.h

Copyright(C) 2000-2012, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapWSDualHttpBinding_USCOREICalculatorDuplexProxy_H
#define soapWSDualHttpBinding_USCOREICalculatorDuplexProxy_H
#include "soapH.h"

class SOAP_CMAC WSDualHttpBinding_USCOREICalculatorDuplexProxy
{ public:
	struct soap *soap;
	bool own;
	/// Endpoint URL of service 'WSDualHttpBinding_USCOREICalculatorDuplexProxy' (change as needed)
	const char *soap_endpoint;
	/// Constructor
	WSDualHttpBinding_USCOREICalculatorDuplexProxy();
	/// Constructor to use/share an engine state
	WSDualHttpBinding_USCOREICalculatorDuplexProxy(struct soap*);
	/// Constructor with endpoint URL
	WSDualHttpBinding_USCOREICalculatorDuplexProxy(const char *url);
	/// Constructor with engine input+output mode control
	WSDualHttpBinding_USCOREICalculatorDuplexProxy(soap_mode iomode);
	/// Constructor with URL and input+output mode control
	WSDualHttpBinding_USCOREICalculatorDuplexProxy(const char *url, soap_mode iomode);
	/// Constructor with engine input and output mode control
	WSDualHttpBinding_USCOREICalculatorDuplexProxy(soap_mode imode, soap_mode omode);
	/// Destructor frees deserialized data
	virtual	~WSDualHttpBinding_USCOREICalculatorDuplexProxy();
	/// Initializer used by constructors
	virtual	void WSDualHttpBinding_USCOREICalculatorDuplexProxy_init(soap_mode imode, soap_mode omode);
	/// Delete all deserialized data (uses soap_destroy and soap_end)
	virtual	void destroy();
	/// Delete all deserialized data and reset to default
	virtual	void reset();
	/// Disables and removes SOAP Header from message
	virtual	void soap_noheader();
	/// Put SOAP Header in message
	virtual	void soap_header(char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance, struct wsrm__SequenceType *wsrm__Sequence, int __sizeAckRequested, struct wsrm__AckRequestedType *wsrm__AckRequested, int __sizeSequenceAcknowledgement, struct _wsrm__SequenceAcknowledgement *wsrm__SequenceAcknowledgement, struct wsrm__SequenceFaultType *wsrm__SequenceFault);
	/// Get SOAP Header structure (NULL when absent)
	virtual	const SOAP_ENV__Header *soap_header();
	/// Get SOAP Fault structure (NULL when absent)
	virtual	const SOAP_ENV__Fault *soap_fault();
	/// Get SOAP Fault string (NULL when absent)
	virtual	const char *soap_fault_string();
	/// Get SOAP Fault detail as string (NULL when absent)
	virtual	const char *soap_fault_detail();
	/// Close connection (normally automatic, except for send_X ops)
	virtual	int soap_close_socket();
	/// Force close connection (can kill a thread blocked on IO)
	virtual	int soap_force_close_socket();
	/// Print fault
	virtual	void soap_print_fault(FILE*);
#ifndef WITH_LEAN
	/// Print fault to stream
#ifndef WITH_COMPAT
	virtual	void soap_stream_fault(std::ostream&);
#endif

	/// Put fault into buffer
	virtual	char *soap_sprint_fault(char *buf, size_t len);
#endif

	/// Web service one-way send operation 'send_Clear' (returns error code or SOAP_OK)
	virtual	int send_Clear(_mssadh__Clear *mssadh__Clear) { return this->send_Clear(NULL, NULL, mssadh__Clear); }
	virtual	int send_Clear(const char *endpoint, const char *soap_action, _mssadh__Clear *mssadh__Clear);
	/// Web service one-way receive operation 'recv_Clear' (returns error code or SOAP_OK);
	virtual	int recv_Clear(struct __tempuri__Clear&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_Clear' (returns error code or SOAP_OK)
	virtual	int recv_Clear_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'Clear' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int Clear(_mssadh__Clear *mssadh__Clear) { return this->Clear(NULL, NULL, mssadh__Clear); }
	virtual	int Clear(const char *endpoint, const char *soap_action, _mssadh__Clear *mssadh__Clear) { if (this->send_Clear(endpoint, soap_action, mssadh__Clear) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }

	/// Web service one-way send operation 'send_AddTo' (returns error code or SOAP_OK)
	virtual	int send_AddTo(_mssadh__AddTo *mssadh__AddTo) { return this->send_AddTo(NULL, NULL, mssadh__AddTo); }
	virtual	int send_AddTo(const char *endpoint, const char *soap_action, _mssadh__AddTo *mssadh__AddTo);
	/// Web service one-way receive operation 'recv_AddTo' (returns error code or SOAP_OK);
	virtual	int recv_AddTo(struct __tempuri__AddTo&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_AddTo' (returns error code or SOAP_OK)
	virtual	int recv_AddTo_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'AddTo' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int AddTo(_mssadh__AddTo *mssadh__AddTo) { return this->AddTo(NULL, NULL, mssadh__AddTo); }
	virtual	int AddTo(const char *endpoint, const char *soap_action, _mssadh__AddTo *mssadh__AddTo) { if (this->send_AddTo(endpoint, soap_action, mssadh__AddTo) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }

	/// Web service one-way send operation 'send_SubtractFrom' (returns error code or SOAP_OK)
	virtual	int send_SubtractFrom(_mssadh__SubtractFrom *mssadh__SubtractFrom) { return this->send_SubtractFrom(NULL, NULL, mssadh__SubtractFrom); }
	virtual	int send_SubtractFrom(const char *endpoint, const char *soap_action, _mssadh__SubtractFrom *mssadh__SubtractFrom);
	/// Web service one-way receive operation 'recv_SubtractFrom' (returns error code or SOAP_OK);
	virtual	int recv_SubtractFrom(struct __tempuri__SubtractFrom&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_SubtractFrom' (returns error code or SOAP_OK)
	virtual	int recv_SubtractFrom_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'SubtractFrom' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int SubtractFrom(_mssadh__SubtractFrom *mssadh__SubtractFrom) { return this->SubtractFrom(NULL, NULL, mssadh__SubtractFrom); }
	virtual	int SubtractFrom(const char *endpoint, const char *soap_action, _mssadh__SubtractFrom *mssadh__SubtractFrom) { if (this->send_SubtractFrom(endpoint, soap_action, mssadh__SubtractFrom) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }

	/// Web service one-way send operation 'send_MultiplyBy' (returns error code or SOAP_OK)
	virtual	int send_MultiplyBy(_mssadh__MultiplyBy *mssadh__MultiplyBy) { return this->send_MultiplyBy(NULL, NULL, mssadh__MultiplyBy); }
	virtual	int send_MultiplyBy(const char *endpoint, const char *soap_action, _mssadh__MultiplyBy *mssadh__MultiplyBy);
	/// Web service one-way receive operation 'recv_MultiplyBy' (returns error code or SOAP_OK);
	virtual	int recv_MultiplyBy(struct __tempuri__MultiplyBy&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_MultiplyBy' (returns error code or SOAP_OK)
	virtual	int recv_MultiplyBy_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'MultiplyBy' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int MultiplyBy(_mssadh__MultiplyBy *mssadh__MultiplyBy) { return this->MultiplyBy(NULL, NULL, mssadh__MultiplyBy); }
	virtual	int MultiplyBy(const char *endpoint, const char *soap_action, _mssadh__MultiplyBy *mssadh__MultiplyBy) { if (this->send_MultiplyBy(endpoint, soap_action, mssadh__MultiplyBy) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }

	/// Web service one-way send operation 'send_DivideBy' (returns error code or SOAP_OK)
	virtual	int send_DivideBy(_mssadh__DivideBy *mssadh__DivideBy) { return this->send_DivideBy(NULL, NULL, mssadh__DivideBy); }
	virtual	int send_DivideBy(const char *endpoint, const char *soap_action, _mssadh__DivideBy *mssadh__DivideBy);
	/// Web service one-way receive operation 'recv_DivideBy' (returns error code or SOAP_OK);
	virtual	int recv_DivideBy(struct __tempuri__DivideBy&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_DivideBy' (returns error code or SOAP_OK)
	virtual	int recv_DivideBy_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'DivideBy' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int DivideBy(_mssadh__DivideBy *mssadh__DivideBy) { return this->DivideBy(NULL, NULL, mssadh__DivideBy); }
	virtual	int DivideBy(const char *endpoint, const char *soap_action, _mssadh__DivideBy *mssadh__DivideBy) { if (this->send_DivideBy(endpoint, soap_action, mssadh__DivideBy) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }

	/// Web service one-way send operation 'send_Result' (returns error code or SOAP_OK)
	virtual	int send_Result(_mssadh__Result *mssadh__Result) { return this->send_Result(NULL, NULL, mssadh__Result); }
	virtual	int send_Result(const char *endpoint, const char *soap_action, _mssadh__Result *mssadh__Result);
	/// Web service one-way receive operation 'recv_Result' (returns error code or SOAP_OK);
	virtual	int recv_Result(struct __tempuri__Result&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_Result' (returns error code or SOAP_OK)
	virtual	int recv_Result_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'Result' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int Result(_mssadh__Result *mssadh__Result) { return this->Result(NULL, NULL, mssadh__Result); }
	virtual	int Result(const char *endpoint, const char *soap_action, _mssadh__Result *mssadh__Result) { if (this->send_Result(endpoint, soap_action, mssadh__Result) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }

	/// Web service one-way send operation 'send_Equation' (returns error code or SOAP_OK)
	virtual	int send_Equation(_mssadh__Equation *mssadh__Equation) { return this->send_Equation(NULL, NULL, mssadh__Equation); }
	virtual	int send_Equation(const char *endpoint, const char *soap_action, _mssadh__Equation *mssadh__Equation);
	/// Web service one-way receive operation 'recv_Equation' (returns error code or SOAP_OK);
	virtual	int recv_Equation(struct __tempuri__Equation&);
	/// Web service receive of HTTP Accept acknowledgment for one-way send operation 'send_Equation' (returns error code or SOAP_OK)
	virtual	int recv_Equation_empty_response() { return soap_recv_empty_response(this->soap); }
	/// Web service one-way synchronous send operation 'Equation' with HTTP Accept/OK response receive (returns error code or SOAP_OK)
	virtual	int Equation(_mssadh__Equation *mssadh__Equation) { return this->Equation(NULL, NULL, mssadh__Equation); }
	virtual	int Equation(const char *endpoint, const char *soap_action, _mssadh__Equation *mssadh__Equation) { if (this->send_Equation(endpoint, soap_action, mssadh__Equation) || soap_recv_empty_response(this->soap)) return this->soap->error; return SOAP_OK; }
};
#endif
