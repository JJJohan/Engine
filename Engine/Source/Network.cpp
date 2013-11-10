#include "stdafx.h"
#include "libcurl\curl.h"

namespace Engine
{
	Network& Network::Instance()
	{
		static Network instance;
		return instance;
	}

	struct Network::Web::String
	{
		char *ptr;
		size_t len;
	};

	struct Network::Web::Session
	{
		Session()
		{
			session = (CURL*)curl_easy_init();
			curl_easy_setopt(session, CURLOPT_COOKIEFILE, "");
			curl_easy_setopt(session, CURLOPT_COOKIEJAR, "");
		}

		~Session()
		{
			curl_easy_cleanup(session);
		}

		CURL* session;
	};

	Network::Network()
	{
	}

	Network::~Network()
	{
	}

	Network::Web::Web()
	{
	}

	Network::Web::~Web()
	{
		// Clean up active sessions
		for (auto it = m_sessions.begin(); it != m_sessions.end();)
		{
			curl_easy_cleanup((*it)->session);
			Session* data = *it;
			it = m_sessions.erase(it);
			SAFE_DELETE(data);
		}
	}

	Network::Web::Async::Async()
	{
	}

	Network::Web::Async::~Async()
	{
		// Clean up active threads
		for (auto it = m_threadPool.begin(); it != m_threadPool.end();)
		{
			(*it).join();
			it = m_threadPool.erase(it);
		}
	}

	void Network::Web::init_string(struct Network::Web::String *s) 
	{
		s->len = 0;
		s->ptr = (char*)malloc(s->len + 1);
		if (s->ptr == NULL) {
			fprintf(stderr, "malloc() failed\n");
			exit(EXIT_FAILURE);
		}
		s->ptr[0] = '\0';
	}

	size_t writefunc(void *ptr, size_t size, size_t nmemb, struct Network::Web::String *s)
	{
		size_t new_len = s->len + size*nmemb;
		s->ptr = (char*)realloc(s->ptr, new_len + 1);
		if (s->ptr == NULL) {
			fprintf(stderr, "realloc() failed\n");
			exit(EXIT_FAILURE);
		}
		memcpy(s->ptr + s->len, ptr, size*nmemb);
		s->ptr[new_len] = '\0';
		s->len = new_len;

		return size*nmemb;
	}

	bool Network::Web::Get(std::string a_url, std::string& a_output, Session* a_session)
	{
		bool tempSession = false;
		if (a_session == NULL)
		{
			tempSession = true;
			a_session = new Session();
		}	

		if (a_session->session)
		{
			// Init string buffer
			struct Network::Web::String s;
			init_string(&s);

			// Perform request
			curl_easy_setopt(a_session->session, CURLOPT_URL, a_url.c_str());
			curl_easy_setopt(a_session->session, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(a_session->session, CURLOPT_WRITEDATA, &s);
			if (curl_easy_perform(a_session->session) != CURLE_OK)
			{
				LOG_WARNING("Get() to '" + a_url + "' failed.");
				curl_easy_cleanup(a_session->session);
				a_session = NULL;
				a_output = "";
				SAFE_DELETE(s.ptr);
				return false;
			}

			// cleanup
			if (tempSession)
				SAFE_DELETE(a_session);

			// Retrieve data
			a_output = s.ptr;
			SAFE_DELETE(s.ptr);
			return true;
		}
		else
		{
			LOG_ERROR("Could not set up CURL interface.");
		}

		return false;
	}

	Network::Web::Session* Network::Web::CreateSession()
	{
		return new Session();
	}

	void Network::Web::EndSession(Session* a_session)
	{
		for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
		{
			if (a_session == (*it))
			{
				curl_easy_cleanup((*it)->session);
				Session* data = *it;
				m_sessions.erase(it);
				SAFE_DELETE(data);
				break;
			}
		}
	}

	bool Network::Web::Post(std::string a_url, std::string a_data, Session* a_session)
	{
		bool tempSession = false;
		if (!a_session)
		{
			tempSession = true;
			a_session = new Session();
		}

		if (a_session->session)
		{
			// Send POST
			curl_easy_setopt(a_session->session, CURLOPT_URL, a_url.c_str());
			curl_easy_setopt(a_session->session, CURLOPT_POSTFIELDS, a_data.c_str());
			if (curl_easy_perform(a_session->session) != CURLE_OK)
			{
				LOG_WARNING("Post() to '" + a_url + "' failed.");
				curl_easy_cleanup(a_session->session);
				a_session = NULL;
				return false;
			}

			// cleanup
			if (tempSession)
				curl_easy_cleanup(a_session->session);

			return true;
		}
		else
		{
			LOG_ERROR("Could not set up CURL interface.");
		}

		return false;
	}

	bool Network::Web::PostGet(std::string a_url, std::string a_data, std::string& a_output, Session* a_session)
	{
		bool tempSession = false;
		if (!a_session)
		{
			tempSession = true;
			a_session = new Session();
		}

		if (a_session->session)
		{
			// Init string buffer
			struct Network::Web::String s;
			init_string(&s);

			// Send POST & GET
			curl_easy_setopt(a_session->session, CURLOPT_URL, a_url.c_str());
			curl_easy_setopt(a_session->session, CURLOPT_POSTFIELDS, a_data.c_str());
			curl_easy_setopt(a_session->session, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(a_session->session, CURLOPT_WRITEDATA, &s);
			if (curl_easy_perform(a_session->session) != CURLE_OK)
			{
				LOG_WARNING("PostGet() to '" + a_url + "' failed.");
				curl_easy_cleanup(a_session->session);
				a_session = NULL;
				a_output = "";
				SAFE_DELETE(s.ptr);
				return false;
			}		

			// cleanup
			if (tempSession)
				curl_easy_cleanup(a_session->session);

			// Retrieve data
			a_output = s.ptr;
			SAFE_DELETE(s.ptr);
			return true;
		}
		else
		{
			LOG_ERROR("Could not set up CURL interface.");
		}

		return false;
	}

	WEB_RESULT Network::Web::Async::Get(std::string a_url)
	{
		WEB_RESULT result = new Result();
		m_threadPool.push_back(std::thread(&Network::Web::Async::tGet, this, result, a_url));
		return result;
	}
	
	void Network::Web::Async::tGet(WEB_RESULT a_result, std::string a_url)
	{
		Session session;
		if (session.session)
		{
			// Init string buffer
			struct Network::Web::String s;
			Network::Instance().web.init_string(&s);

			// Perform request
			curl_easy_setopt(session.session, CURLOPT_URL, a_url.c_str());
			curl_easy_setopt(session.session, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(session.session, CURLOPT_WRITEDATA, &s);
			if (curl_easy_perform(session.session) != CURLE_OK)
			{
				LOG_WARNING("Async::Get() to '" + a_url + "' failed.");
				curl_easy_cleanup(session.session);
				SAFE_DELETE(s.ptr);
				a_result->complete = true;
				a_result->success = false;
				return;
			}

			// Retrieve data
			a_result->data = s.ptr;
			a_result->complete = true;
			a_result->success = true;
			SAFE_DELETE(s.ptr);
			return;
		}
		else
		{
			LOG_ERROR("Could not set up CURL interface.");
		}

		a_result->complete = true;
		a_result->success = false;
		return;
	}

	WEB_RESULT Network::Web::Async::Post(std::string a_url, std::string a_data)
	{
		WEB_RESULT result = new Result();
		m_threadPool.push_back(std::thread(&Network::Web::Async::tPost, this, result, a_url, a_data));
		return result;
	}

	void Network::Web::Async::tPost(WEB_RESULT a_result, std::string a_url, std::string a_data)
	{
		Session session;
		if (session.session)
		{
			// Perform request
			curl_easy_setopt(session.session, CURLOPT_URL, a_url.c_str());
			curl_easy_setopt(session.session, CURLOPT_POSTFIELDS, a_data.c_str());
			if (curl_easy_perform(session.session) != CURLE_OK)
			{
				LOG_WARNING("Async::Post() to '" + a_url + "' failed.");
				curl_easy_cleanup(session.session);
				a_result->complete = true;
				a_result->success = false;
				return;
			}

			// Retrieve data
			a_result->complete = true;
			a_result->success = true;
			return;
		}
		else
		{
			LOG_ERROR("Could not set up CURL interface.");
		}

		a_result->complete = true;
		a_result->success = false;
		return;
	}

	WEB_RESULT Network::Web::Async::PostGet(std::string a_url, std::string a_data)
	{
		WEB_RESULT result = new Result();
		m_threadPool.push_back(std::thread(&Network::Web::Async::tPostGet, this, result, a_url, a_data));
		return result;
	}

	void Network::Web::Async::tPostGet(WEB_RESULT a_result, std::string a_url, std::string a_data)
	{
		Session session;
		if (session.session)
		{
			// Init string buffer
			struct Network::Web::String s;
			Network::Instance().web.init_string(&s);

			// Perform request
			curl_easy_setopt(session.session, CURLOPT_URL, a_url.c_str());
			curl_easy_setopt(session.session, CURLOPT_POSTFIELDS, a_data.c_str());
			curl_easy_setopt(session.session, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(session.session, CURLOPT_WRITEDATA, &s);
			if (curl_easy_perform(session.session) != CURLE_OK)
			{
				LOG_WARNING("Async::PostGet() to '" + a_url + "' failed.");
				curl_easy_cleanup(session.session);
				SAFE_DELETE(s.ptr);
				a_result->complete = true;
				a_result->success = false;
				return;
			}

			// Retrieve data
			a_result->data = s.ptr;
			a_result->complete = true;
			a_result->success = true;
			SAFE_DELETE(s.ptr);
			return;
		}
		else
		{
			LOG_ERROR("Could not set up CURL interface.");
		}

		a_result->complete = true;
		a_result->success = false;
		return;
	}
}