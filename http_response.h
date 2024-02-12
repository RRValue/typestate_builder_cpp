#pragma once

#include <string>
#include <vector>
#include <iostream>

// data we want to edit
struct HttpResponseData
{
    std::string status;
    std::vector<std::string> header;
    std::string body;
};

// states
struct Start {};
struct Status {};
struct Headers {};
struct Body {};
struct Send {};

// concept storing all states we want to deal with.
template<class T>
concept BuildState =
    std::is_base_of<T, Start>::value || //
    std::is_base_of<T, Status>::value || //
    std::is_base_of<T, Headers>::value || //
    std::is_base_of<T, Body>::value || //
    std::is_base_of<T, Send>::value;

// forward decl to let HttpResponseBuilderCommon know HttpResponseBuilder
template<BuildState S>
struct HttpResponseBuilder;

// use crtp to have a general method struct, which knows the derived types and can used within every typestate.
template<class T>
class HttpResponseBuilderCommon
{
public:
    HttpResponseBuilderCommon() : data{ "",{},""}
    {
    }
    
    HttpResponseBuilderCommon(HttpResponseData&& d) : data{ std::move(d) }
    {
    }
    
    HttpResponseBuilder<T> print()
    {
        std::cout << "Status: " << data.status << std::endl;
        std::cout << "Headers:" << std::endl;
        for (const auto& header : data.header)
            std::cout << "  Header: " << header << std::endl;
        std::cout << "Body: " << data.body << std::endl;

        return {std::move(data)};
    }

protected:
    template<class U>
    HttpResponseBuilder<U> into()
    {
        return HttpResponseBuilder<U>{std::move(data)};
    }

protected:
    HttpResponseData data;
};

//! general impl do not work, because onyl the specialized version are instantiated.
template<BuildState S>
struct HttpResponseBuilder : HttpResponseBuilderCommon<HttpResponseBuilder<S>>
{
};

template<>
struct HttpResponseBuilder<Send> : HttpResponseBuilderCommon<Send>
{    
    void send() {
        std::cout << "Ab gehts" << std::endl;
    }
};

template<>
struct HttpResponseBuilder<Body> : HttpResponseBuilderCommon<Body>
{
    HttpResponseBuilder<Send> body(std::string body)
    {
        data.body = body;
        
        return into<Send>();
    }
};

template<>
struct HttpResponseBuilder<Headers> : HttpResponseBuilderCommon<Headers>
{    
    HttpResponseBuilder<Headers> add_header(std::string header)
    {
        data.header.push_back(header);

        return into<Headers>();
    }

    HttpResponseBuilder<Body> header_done()
    {
        return into<Body>();
    }
};

template<>
struct HttpResponseBuilder<Status> : HttpResponseBuilderCommon<Status>
{    
    HttpResponseBuilder<Headers> status(std::string status)
    {
        data.status = status;

        return into<Headers>();
    }
};

template<>
struct HttpResponseBuilder<Start> : HttpResponseBuilderCommon<Start>
{   
    static HttpResponseBuilder<Status> create()
    {
        return {};
    }
};