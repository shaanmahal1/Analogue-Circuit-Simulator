// PHYS 30762 Programming in C++
// Project

// Program which simulates different types of RLC circuits.
// The program prompts the user to choose a circuit type and enter values for 
// the components. The total impedance and phase difference is calculated as well
// as the individual component impedances and phase shifts.

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <iomanip>
#include <string>
#include <memory>
#include <limits>

const double pi = 3.14159265358979323846;

// Abstract base class for components in circuit
class components
{
protected:
	double frequency;
	std::complex<double> impedance;
public:
	components() : frequency(0) {}
	virtual ~components() {}
	virtual std::string get_type() const = 0;
	virtual void set_frequency(double f) = 0;
	virtual double get_frequency() const = 0;
	virtual std::complex<double> get_impedance() const = 0;
	virtual double get_impedance_magnitude() const = 0;
	virtual double get_phase_difference() const = 0;
};

// Derived class for resistor
class resistor : public components
{
private: 
	double resistance;
public:
	resistor(double r) : resistance(r) {impedance = resistance;}
	~resistor() {}
	void set_frequency(double f) override {
		frequency = f;
	}
	double get_frequency() const override {
		return frequency;
	}
	std::complex<double> get_impedance() const override {
		return { impedance };
	}
	double get_impedance_magnitude() const override {
		return std::abs(impedance);
	}
	double get_phase_difference() const override {
		return std::arg(impedance);
	}
	std::string get_type() const override {
		return "Resistor";
	}
};

// Derived class for capacitor
class capacitor : public components
{
private:
	double capacitance;
public:
	capacitor(double c) : capacitance(c) { impedance = std::complex<double>(0, -1.0 / (2.0 * pi * frequency * capacitance)); }
	~capacitor() {}
	void set_frequency(double f) override {
		frequency = f;
		impedance = std::complex<double>(0, -1.0 / (2 * pi * frequency * capacitance));
	}
	double get_frequency() const override {
		return frequency;
	}
	std::complex<double> get_impedance() const override {
		return impedance;
	}
	double get_impedance_magnitude() const override {
		return std::abs(impedance);
	}
	double get_phase_difference() const override {
		return -pi/2;
	}
	std::string get_type() const override {
		return "Capacitor";
	}
};

// Derived class for inductor
class inductor : public components
{
private:
	double inductance;
public:
	inductor(double l) : inductance(l) { impedance = std::complex<double>(0, 2 * pi * inductance); }
	~inductor() {}
	void set_frequency(double f) override {
		frequency = f;
		impedance = std::complex<double>(0, 2 * pi * frequency * inductance);
	}
	double get_frequency() const override {
		return frequency;
	}
	std::complex<double> get_impedance() const override {
		return impedance;
	}
	double get_impedance_magnitude() const override {
		return std::abs(impedance);
	}
	double get_phase_difference() const override {
		return pi/2;
	}
	std::string get_type() const override {
		return "Inductor";
	}
};

// Derived class for diode
class diode : public components
{
private:
	double capacitance;
	double resistance;
	double saturation_current;
public:
	diode(double c, double r, double is) : capacitance(c), resistance(r), saturation_current(is)
	{
		impedance = std::complex<double>(0, resistance);
	}
	~diode() {}
	void set_frequency(double f) override {
		frequency = f;
		// Calculate impedance of diode based on frequency
		double omega_c = 1.0 / (resistance * capacitance);
		double omega_s = (saturation_current * resistance) / capacitance;
		double j = std::sqrt(-1);
		double denom = 1.0 + (j * 2*pi*f * capacitance * resistance);
		impedance = std::complex<double>(resistance / denom, omega_s / (j * 2*pi*f * capacitance * denom));
	}
	double get_frequency() const override {
		return frequency;
	}
	std::complex<double> get_impedance() const override {
		return impedance;
	}
	double get_impedance_magnitude() const override {
		return std::abs(impedance);
	}
	double get_phase_difference() const override {
		return std::arg(impedance);
	}
	std::string get_type() const override {
		return "Diode";
	}
};

// Derived class for transistor
class transistor : public components
{
private:
	double collector_current;
	double base_current;
	double emitter_current;
	double collector_emitter_voltage;
	double base_emitter_voltage;
public:
	transistor(double cc, double bc, double ec, double cev, double bev) :
		collector_current(cc), base_current(bc), emitter_current(ec),
		collector_emitter_voltage(cev), base_emitter_voltage(bev) {
		// Calculate transistor impedance at frequency = 0
		impedance = std::complex<double>(collector_emitter_voltage / collector_current, 0.0);
	}
	~transistor() {}
	void set_frequency(double f) override {
		// Transistor impedance does not depend on frequency
	}
	double get_frequency() const override {
		return 0.0;
	}
	std::complex<double> get_impedance() const override {
		return impedance;
	}
	double get_impedance_magnitude() const override {
		return std::abs(impedance);
	}
	double get_phase_difference() const override {
		return 0.0;
	}
	std::string get_type() const override {
		return "Transistor";
	}
};

// Generic circuit class which stores total impedance and phase difference of the whole circuit
class circuit 
{
private:
	std::vector<components*> component_list; 
	std::complex<double> total_impedance; 
	double frequency;
public:
	circuit() : total_impedance(0.0), frequency(0.0) {}
	~circuit() {}
	void add_component_in_series(components* component) {
		component_list.push_back(component);
		update_impedance_series();
	}
	void add_component_in_parallel(components* component) {
		component_list.push_back(component);
		update_impedance_parallel();
	}
	void set_frequency(double f) { 
		frequency = f;
		update_impedance_series(); // Recalculate total impedance for components in series
		update_impedance_parallel(); // Recalculate total impedance for components in parallel
	}
	void update_impedance_series() {
		total_impedance = 0;
		for (const auto& component : component_list) {
			total_impedance += component->get_impedance();
		}
	}
	void update_impedance_parallel() {
		total_impedance = 0;
		for (const auto& component : component_list) {
			total_impedance += 1.0 / component->get_impedance();
		}
		total_impedance = 1.0 / total_impedance;
	}
	std::complex<double> get_circuit_impedance() const {
		return total_impedance;
	}
	double get_total_impedance_magntiude() const {
		return std::abs(total_impedance);
	}
	double get_phase_difference() const {
		return std::arg(total_impedance);
	}
};

int main()
{
	std::vector<std::unique_ptr<components>> component_library;

	int circuit_type;
	double freq;
	double r_value, c_value, l_value;

	try {
		std::cout << "Choose circuit type: " << std::endl;
		std::cout << "1. Parallel RLC circuit" << std::endl;
		std::cout << "2. Series RLC circuit" << std::endl;
		std::cout << "3. RL in Series" << std::endl;
		std::cout << "4. RL in Parallel" << std::endl;
		std::cout << "5. RC in Series" << std::endl;
		std::cout << "6. RC in Parallel" << std::endl;
		std::cout << "7. LC in Series" << std::endl;
		std::cout << "8. LC in Parallel" << std::endl;
		
		// Validate input for circuit type
		while (!(std::cin >> circuit_type) || circuit_type < 1 || circuit_type > 8 || std::cin.peek() != '\n') {
			std::cin.clear();  // Clear error flags
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
			std::cout << "Error: Invalid circuit type. Please enter an integer between 1 and 8." << std::endl;
		}
 
		std::cout << "Frequency (Hz): ";
		// Validate input for frequency
		while (!(std::cin >> freq) || freq == 0 || freq < 0 || std::cin.peek() != '\n') {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Error: Invalid frequency. Please enter a valid number." << std::endl;
		}

		if (circuit_type == 1) {
			circuit example_circuit;

			std::cout << "Enter resistance value (Ohms): ";
			// Validate input for resistance
			while (!(std::cin >> r_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid resistance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter capacitance value (Farads): ";
			// Validate input for capacitance
			while (!(std::cin >> c_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid capacitance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter inductance value (Henry): ";
			// Validate input for inductance 
			while (!(std::cin >> l_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid inductance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			resistor r(r_value);
			capacitor c(c_value);
			inductor l(l_value);

			example_circuit.add_component_in_parallel(&r);
			example_circuit.add_component_in_parallel(&c);
			example_circuit.add_component_in_parallel(&l);

			r.set_frequency(freq);
			c.set_frequency(freq);
			l.set_frequency(freq);
			example_circuit.set_frequency(freq);

			component_library.push_back(std::make_unique<resistor>(r));
			component_library.push_back(std::make_unique<capacitor>(c));
			component_library.push_back(std::make_unique<inductor>(l));

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----R-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----C-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----L-----+" << std::endl;
		}
		else if (circuit_type == 2) {
			circuit example_circuit2;

			std::cout << "Enter resistance value (Ohms): ";
			while (!(std::cin >> r_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid resistance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter capacitance value (Farads): ";
			while (!(std::cin >> c_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid capacitance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter inductance value (Henry): ";
			while (!(std::cin >> l_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid inductance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			resistor r(r_value);
			capacitor c(c_value);
			inductor l(l_value);

			example_circuit2.add_component_in_series(&r);
			example_circuit2.add_component_in_series(&c);
			example_circuit2.add_component_in_series(&l);

			r.set_frequency(freq);
			c.set_frequency(freq);
			l.set_frequency(freq);
			example_circuit2.set_frequency(freq);

			component_library.push_back(std::make_unique<resistor>(r));
			component_library.push_back(std::make_unique<capacitor>(c));;
			component_library.push_back(std::make_unique<inductor>(l));

			example_circuit2.update_impedance_series();

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit2.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit2.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----R-----C-----L-----+" << std::endl;
			std::cout << "|                       |" << std::endl;
			std::cout << "+-----------------------+" << std::endl;
		}
		else if (circuit_type == 3) {
			circuit example_circuit3;

			std::cout << "Enter resistance value (Ohms): ";
			while (!(std::cin >> r_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid resistance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter inductance value (Henry): ";
			while (!(std::cin >> l_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid inductance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			resistor r(r_value);
			inductor l(l_value);

			example_circuit3.add_component_in_series(&r);
			example_circuit3.add_component_in_series(&l);

			r.set_frequency(freq);
			l.set_frequency(freq);
			example_circuit3.set_frequency(freq);

			component_library.push_back(std::make_unique<resistor>(r));
			component_library.push_back(std::make_unique<inductor>(l));

			example_circuit3.update_impedance_series();

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit3.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit3.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----R-----L-----+" << std::endl;
			std::cout << "|                 |" << std::endl;
			std::cout << "+-----------------+" << std::endl;
		}
		else if (circuit_type == 4) {
			circuit example_circuit4;

			std::cout << "Enter resistance value (Ohms): ";
			while (!(std::cin >> r_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid resistance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter inductance value (Henry): ";
			while (!(std::cin >> l_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid inductance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			resistor r(r_value);
			inductor l(l_value);

			example_circuit4.add_component_in_parallel(&r);
			example_circuit4.add_component_in_parallel(&l);

			r.set_frequency(freq);
			l.set_frequency(freq);
			example_circuit4.set_frequency(freq);

			component_library.push_back(std::make_unique<resistor>(r));
			component_library.push_back(std::make_unique<inductor>(l));

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit4.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit4.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----R-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----L-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----------+" << std::endl;
		}
		else if (circuit_type == 5) {
			circuit example_circuit5;

			std::cout << "Enter resistance value (Ohms): ";
			while (!(std::cin >> r_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid resistance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter capacitance value (Farads): ";
			while (!(std::cin >> c_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid capacitance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			resistor r(r_value);
			capacitor c(c_value);

			example_circuit5.add_component_in_series(&r);
			example_circuit5.add_component_in_series(&c);

			r.set_frequency(freq);
			c.set_frequency(freq);
			example_circuit5.set_frequency(freq);

			component_library.push_back(std::make_unique<resistor>(r));
			component_library.push_back(std::make_unique<capacitor>(c));

			example_circuit5.update_impedance_series();

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit5.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit5.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----R-----C-----+" << std::endl;
			std::cout << "|                 |" << std::endl;
			std::cout << "+-----------------+" << std::endl;
		}
		else if (circuit_type == 6) {
			circuit example_circuit6;

			std::cout << "Enter resistance value (Ohms): ";
			while (!(std::cin >> r_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid resistance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter capacitance value (Farads): ";
			while (!(std::cin >> c_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid capacitance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			resistor r(r_value);
			capacitor c(c_value);

			example_circuit6.add_component_in_parallel(&r);
			example_circuit6.add_component_in_parallel(&c);

			r.set_frequency(freq);
			c.set_frequency(freq);
			example_circuit6.set_frequency(freq);

			component_library.push_back(std::make_unique<resistor>(r));
			component_library.push_back(std::make_unique<capacitor>(c));

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit6.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit6.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----R-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----C-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----------+" << std::endl;
		}
		else if (circuit_type == 7) {
			circuit example_circuit7;

			std::cout << "Enter capacitance value (Farads): ";
			while (!(std::cin >> c_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid capacitance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter inductance value (Henry): ";
			while (!(std::cin >> l_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid inductance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			capacitor c(c_value);
			inductor l(l_value);

			example_circuit7.add_component_in_series(&l);
			example_circuit7.add_component_in_series(&c);

			l.set_frequency(freq);
			c.set_frequency(freq);
			example_circuit7.set_frequency(freq);

			component_library.push_back(std::make_unique<inductor>(l));
			component_library.push_back(std::make_unique<capacitor>(c));

			example_circuit7.update_impedance_series();

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit7.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit7.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----L-----C-----+" << std::endl;
			std::cout << "|                 |" << std::endl;
			std::cout << "+-----------------+" << std::endl;
		}
		else if (circuit_type == 8) {
			circuit example_circuit8;

			std::cout << "Enter capacitance value (Farads): ";
			while (!(std::cin >> c_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid capacitance value. Please enter a valid number." << std::endl;
			}

			std::cout << "Enter inductance value (Henry): ";
			while (!(std::cin >> l_value) || std::cin.peek() != '\n') {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Error: Invalid inductance value. Please enter a valid number." << std::endl;
			}
			std::cout << std::endl;

			capacitor c(c_value);
			inductor l(l_value);

			example_circuit8.add_component_in_parallel(&l);
			example_circuit8.add_component_in_parallel(&c);

			l.set_frequency(freq);
			c.set_frequency(freq);
			example_circuit8.set_frequency(freq);

			component_library.push_back(std::make_unique<inductor>(l));
			component_library.push_back(std::make_unique<capacitor>(c));

			std::cout << "Total Impedance Magnitude at " << freq << "Hz: " << example_circuit8.get_total_impedance_magntiude() << " Ohms" << std::endl;
			std::cout << "Total Phase Difference: " << example_circuit8.get_phase_difference() << " rad" << std::endl;
			std::cout << std::endl;

			std::cout << "Component Impedances and Phase Shifts:" << std::endl;
			for (auto& component : component_library) {
				std::cout << "Type: " << component->get_type() << std::endl;
				std::cout << "Impedance Magnitude: " << component->get_impedance_magnitude() << " Ohms" << std::endl;
				std::cout << "Phase Shift: " << component->get_phase_difference() << " rad" << std::endl;
				std::cout << std::endl;
			}
			std::cout << "Circuit Diagram: " << std::endl;
			std::cout << "+-----L-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----C-----+" << std::endl;
			std::cout << "|           |" << std::endl;
			std::cout << "+-----------+" << std::endl;
		}
	}
	catch (const std::exception& ex) {
		// Catch exceptions and handle them
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return 0;
}
