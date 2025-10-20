#include <iostream>
#include <iomanip>
#include <cassert>

using std::cout;
using std::cin;
using std::endl;
using std::setw;

// --- Clase Base Abstracta ---
template <typename T>
class MatrizBase {
protected:
    int _filas;
    int _columnas;

public:
    MatrizBase(int f = 0, int c = 0) : _filas(f), _columnas(c) {}
    virtual ~MatrizBase() = default;

    int filas() const { return _filas; }
    int columnas() const { return _columnas; }

    // Interfaz pública para obtener/asignar un elemento (para uso desde main o desde otras matrices)
    virtual T obtener(int i, int j) const = 0;
    virtual void asignar(int i, int j, T val) = 0;

    // Operaciones requeridas
    virtual void cargarValores() = 0;
    // sumar devuelve un nuevo objeto creado con new (del mismo tipo concreto que *this)
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;
    virtual void imprimir() const = 0;
};

// --- Matriz Dinámica: Rule of Five, T** ---
template <typename T>
class MatrizDinamica : public MatrizBase<T> {
private:
    T **_datos;

    void reservar(int f, int c) {
        if (f <= 0 || c <= 0) {
            _datos = nullptr;
            return;
        }
        _datos = new T*[f];
        for (int i = 0; i < f; ++i) {
            _datos[i] = new T[c];
        }
    }

    void liberar() {
        if (!_datos) return;
        for (int i = 0; i < this->_filas; ++i) {
            delete[] _datos[i];
        }
        delete[] _datos;
        _datos = nullptr;
    }

public:
    // Constructor
    MatrizDinamica(int f = 0, int c = 0) : MatrizBase<T>(f, c), _datos(nullptr) {
        reservar(f, c);
        // inicializar a 0
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j)
                _datos[i][j] = T();
    }

    // Destructor
    ~MatrizDinamica() override {
        liberar();
    }

    // Constructor de copia (deep copy)
    MatrizDinamica(const MatrizDinamica<T>& other) : MatrizBase<T>(other._filas, other._columnas), _datos(nullptr) {
        reservar(this->_filas, this->_columnas);
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j)
                _datos[i][j] = other._datos[i][j];
    }

    // Operador de asignación (copy-and-swap idiom)
    MatrizDinamica<T>& operator=(const MatrizDinamica<T>& other) {
        if (this == &other) return *this;
        // crear una copia temporal
        MatrizDinamica<T> temp(other);
        // swap los miembros
        std::swap(this->_filas, temp._filas);
        std::swap(this->_columnas, temp._columnas);
        std::swap(this->_datos, temp._datos);
        return *this;
    }

    // Move constructor
    MatrizDinamica(MatrizDinamica<T>&& other) noexcept : MatrizBase<T>(other._filas, other._columnas), _datos(other._datos) {
        other._datos = nullptr;
        other._filas = other._columnas = 0;
    }

    // Move assignment
    MatrizDinamica<T>& operator=(MatrizDinamica<T>&& other) noexcept {
        if (this == &other) return *this;
        liberar();
        this->_filas = other._filas;
        this->_columnas = other._columnas;
        _datos = other._datos;
        other._datos = nullptr;
        other._filas = other._columnas = 0;
        return *this;
    }

    // obtener / asignar
    T obtener(int i, int j) const override {
        assert(i >= 0 && i < this->_filas && j >= 0 && j < this->_columnas);
        return _datos[i][j];
    }
    void asignar(int i, int j, T val) override {
        assert(i >= 0 && i < this->_filas && j >= 0 && j < this->_columnas);
        _datos[i][j] = val;
    }

    // cargarValores (interactivo)
    void cargarValores() override {
        cout << "Ingrese valores (" << this->_filas << "x" << this->_columnas << "):\n";
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j) {
                cout << " [" << i << "," << j << "] = ";
                T val;
                cin >> val;
                _datos[i][j] = val;
            }
    }

    // sumar: crea y devuelve nueva MatrizDinamica con new
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        if (this->_filas != otra._filas || this->_columnas != otra._columnas) {
            cout << "Error: dimensiones incompatibles para la suma.\n";
            return nullptr;
        }
        MatrizDinamica<T>* resultado = new MatrizDinamica<T>(this->_filas, this->_columnas);
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j) {
                resultado->asignar(i, j, this->obtener(i, j) + otra.obtener(i, j));
            }
        return resultado;
    }

    void imprimir() const override {
        cout << "| ";
        for (int i = 0; i < this->_filas; ++i) {
            if (i != 0) cout << "\n";
            cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                cout << setw(6) << this->_datos[i][j] << " ";
            }
            cout << "|";
        }
        cout << "\n";
    }
};

// --- Matriz Estática: T _datos[M][N] ---
template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
private:
    T _datos[M][N];

public:
    MatrizEstatica() : MatrizBase<T>(M, N) {
        for (int i = 0; i < M; ++i)
            for (int j = 0; j < N; ++j)
                _datos[i][j] = T();
    }

    // obtener / asignar
    T obtener(int i, int j) const override {
        assert(i >= 0 && i < this->_filas && j >= 0 && j < this->_columnas);
        return _datos[i][j];
    }
    void asignar(int i, int j, T val) override {
        assert(i >= 0 && i < this->_filas && j >= 0 && j < this->_columnas);
        _datos[i][j] = val;
    }

    // cargarValores (interactivo)
    void cargarValores() override {
        cout << "Ingrese valores estaticos (" << this->_filas << "x" << this->_columnas << "):\n";
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j) {
                cout << " [" << i << "," << j << "] = ";
                T val;
                cin >> val;
                _datos[i][j] = val;
            }
    }

    // sumar: devuelve nueva MatrizEstatica<M,N> creada con new (compatible)
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        if (this->_filas != otra._filas || this->_columnas != otra._columnas) {
            cout << "Error: dimensiones incompatibles para la suma (estatica).\n";
            return nullptr;
        }
        MatrizEstatica<T, M, N>* resultado = new MatrizEstatica<T, M, N>();
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j) {
                resultado->asignar(i, j, this->obtener(i, j) + otra.obtener(i, j));
            }
        return resultado;
    }

    void imprimir() const override {
        for (int i = 0; i < this->_filas; ++i) {
            cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                cout << setw(6) << _datos[i][j] << " ";
            }
            cout << "|\n";
        }
    }
};

// --- Sobrecarga del operador + (llama a sumar polimórfico) ---
template <typename T>
MatrizBase<T>* operator+(const MatrizBase<T>& a, const MatrizBase<T>& b) {
    return a.sumar(b);
}

// ----------------- Ejemplo en main -----------------
int main() {
    cout << "\n--- Sistema generico de Algebra Lineal (Demostracion) ---\n\n";

    // DEMO tipo float
    cout << ">> Demostracion (Tipo FLOAT) <<\n\n";

    // 1) Matriz Dinamica A (3x2) a traves de puntero a MatrizBase<float>
    cout << "Creando Matriz Dinamica A (3x2)...\n";
    MatrizBase<float>* A = new MatrizDinamica<float>(3, 2);
    // rellenar manualmente (como el ejemplo)
    A->asignar(0,0,1.5f); A->asignar(0,1,2.0f);
    A->asignar(1,0,0.0f); A->asignar(1,1,1.0f);
    A->asignar(2,0,4.5f); A->asignar(2,1,3.0f);

    cout << "A =\n";
    A->imprimir();
    cout << "\n";

    // 2) Matriz Estatica B (3x2)
    cout << "Creando Matriz Estatica B (3x2)...\n";
    MatrizBase<float>* B = new MatrizEstatica<float,3,2>();
    B->asignar(0,0,0.5f); B->asignar(0,1,1.0f);
    B->asignar(1,0,2.0f); B->asignar(1,1,3.0f);
    B->asignar(2,0,1.0f); B->asignar(2,1,1.0f);

    cout << "B =\n";
    B->imprimir();
    cout << "\n";

    // 3) Operacion polimorfica (suma) - la llamada se realiza sobre A (dinamica) -> resultado dinamico
    cout << "SUMANDO: Matriz C = A + B ...\n";
    MatrizBase<float>* C = (*A) + (*B);
    cout << "\nMatriz Resultado C (3x2, Tipo FLOAT):\n";
    if (C) C->imprimir();
    cout << "\n";

    // liberaciones demostradas
    cout << ">> Demostracion de limpieza de memoria <<\n";
    cout << "Llamando al destructor de C...\n";
    delete C; // si C fue creada (new)
    cout << "Llamando al destructor de A...\n";
    delete A;
    cout << "Llamando al destructor de B...\n";
    delete B;

    // DEMO tipo int
    cout << "\n>> Demostracion (Tipo INT) <<\n\n";
    MatrizBase<int>* Ai = new MatrizDinamica<int>(2,2);
    MatrizBase<int>* Bi = new MatrizEstatica<int,2,2>();

    Ai->asignar(0,0,1); Ai->asignar(0,1,2);
    Ai->asignar(1,0,3); Ai->asignar(1,1,4);

    Bi->asignar(0,0,5); Bi->asignar(0,1,6);
    Bi->asignar(1,0,7); Bi->asignar(1,1,8);

    cout << "Ai =\n"; Ai->imprimir(); cout << "\n";
    cout << "Bi =\n"; Bi->imprimir(); cout << "\n";

    MatrizBase<int>* Ci = (*Ai) + (*Bi);
    cout << "Ai + Bi =\n";
    if (Ci) Ci->imprimir(); cout << "\n";

    delete Ai; delete Bi; delete Ci;

    cout << "Sistema cerrado.\n";
    return 0;
}

