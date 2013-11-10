#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <vector>

#define NETWORK Network::Instance()
#define WEB NETWORK.web
#define WEB_ASYNC WEB.async
#define WEB_RESULT Network::Web::Async::Result*

namespace Engine
{
	class Network
	{
	public:
		static __declspec(dllexport) Network& Instance();

		class Web
		{
		public:
			struct String;
			struct Session;
			
			class Async
			{
			public:
				struct Result
				{
					Result()
					{
						complete = false;
						success = false;
						data = "";
					}

					bool complete;
					bool success;
					std::string data;
				};

				__declspec(dllexport) WEB_RESULT Get(std::string a_url);
				__declspec(dllexport) WEB_RESULT Post(std::string a_url, std::string a_data);
				__declspec(dllexport) WEB_RESULT PostGet(std::string a_url, std::string a_data);

				Async();
				~Async();

			private:
				void tGet(WEB_RESULT a_result, std::string a_url);
				void tPost(WEB_RESULT a_result, std::string a_url, std::string a_data);
				void tPostGet(WEB_RESULT a_result, std::string a_url, std::string a_data);
				std::vector<std::thread> m_threadPool;
			};

			__declspec(dllexport) Session* CreateSession();
			__declspec(dllexport) void EndSession(Session* a_session);
			__declspec(dllexport) bool Get(std::string a_url, std::string& a_output, Session* a_session);
			__declspec(dllexport) bool Post(std::string a_url, std::string a_data, Session* a_session);
			__declspec(dllexport) bool PostGet(std::string a_url, std::string a_data, std::string& a_output, Session* a_session);

			Async async;

			Web();
			~Web();

		protected:		
			void init_string(struct String *s);
	
		private:
			std::vector<Session*> m_sessions;
		};

		Web web;

	private:
		Network();
		~Network();
		Network(Network const&);
		void operator=(Network const&);
	};
}

#endif // NETWORK_H