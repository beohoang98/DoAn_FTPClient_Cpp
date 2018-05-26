#pragma once

namespace std {
	class Output {
		protected:
			virtual void xulyStream() = 0;
			virtual void setToDefault() = 0;
			string data;
			ostream * out;

		public:
			Output(ostream& os);
		
			Output& operator<<(const int);
			Output& operator<<(const string);
			Output& operator<<(const char *);
			Output& operator<<(const char);

			void flush();

			~Output();
	};
}