#include "..\..\octet.h"


namespace octet {

	class stick {
		mesh_instance *mi;
		int timer;
	public:
		stick() {
			timer = 0;
		}

		stick(mesh_instance *mi_) {
			mi = mi_;
			timer = 0;
		}

		mesh_instance& get_mesh_instance() {
			return *mi;
		}

		mesh_instance* getp_mesh_instance() {
			return mi;
		}

		int& get_timer() {
			return timer;
		}
	};
}