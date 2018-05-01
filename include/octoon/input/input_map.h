#ifndef OCTOON_INPUT_MAP_H_
#define OCTOON_INPUT_MAP_H_

#include <octoon/input/input_content.h>

#include <map>
#include <string>

namespace octoon
{
	namespace input
	{

		class InputMap final
		{
		public:
			friend class DefaultInput;

			static void bind(const std::string& id, InputKey::Code code);
			static void bind(const std::string& id, InputButton::Code code);
			static void bind(const std::string& id, const InputOp& ops);

			static void bind(const std::string& id, InputOp ops, InputKey::Code code);
			static void bind(const std::string& id, InputOp ops, InputButton::Code code);

			static void bind(const std::string& id, InputKey::Code code, InputOp ops);
			static void bind(const std::string& id, InputButton::Code code, InputOp ops);

			static void bind(const std::string& id, InputOp ops1, InputOp ops2);

			static void unbind(const std::string& id);
		private:
			InputMap() = delete;
			InputMap(const InputMap&) = delete;

			static std::map<std::string, InputOp> table;
		};
	}
}
#endif // !OCTOON_INPUT_MAP_H_

