cdef extern from "limits.h":
    int c_INT_MIN "INT_MIN"
    int c_INT_MAX "INT_MAX"
    int c_DOUBLE_MIN "DOUBLE_MIN"
    int c_DOUBLE_MAX "DOUBLE_MAX"

cdef extern from "../../src/pythonlab/pyproblem.h":
    cdef cppclass PyProblem:
        PyProblem(int clear)

        void clear()
        void refresh()

        char *getName()
        void setName(char *name)

        char *getCoordinateType()
        void setCoordinateType(char *coordinateType) except +

        char *getMeshType()
        void setMeshType(char *meshType) except +

        char *getMatrixSolver()
        void setMatrixSolver(char *matrixSolver) except +

        double getFrequency()
        void setFrequency(double frequency) except +

        char *getTimeStepMethod()
        void setTimeStepMethod(char *timeStepMethod) except +

        int getTimeMethodOrder()
        void setTimeMethodOrder(int timeMethodOrder) except +

        double getTimeMethodTolerance()
        void setTimeMethodTolerance(double timeMethodTolerance) except +

        double getTimeTotal()
        void setTimeTotal(double timeTotal) except +

        int getNumConstantTimeSteps()
        void setNumConstantTimeSteps(int numConstantTimeSteps) except +

        char *getCouplingType(char *sourceField, char *targetField) except +
        void setCouplingType(char *sourceField, char *targetField, char *type) except +

        void solve()

cdef class __Problem__:
    cdef PyProblem *thisptr

    # Problem(clear)
    def __cinit__(self, int clear = 0):
        self.thisptr = new PyProblem(clear)

    def __dealloc__(self):
        del self.thisptr

    # clear
    def clear(self):
        self.thisptr.clear()

    # refresh
    def refresh(self):
        self.thisptr.refresh()

    # name
    property name:
        def __get__(self):
            return self.thisptr.getName()
        def __set__(self, name):
            self.thisptr.setName(name)

    # coordinate_type
    property coordinate_type:
        def __get__(self):
            return self.thisptr.getCoordinateType()
        def __set__(self, coordinate_type):
            self.thisptr.setCoordinateType(coordinate_type)

    # mesh_type
    property mesh_type:
        def __get__(self):
            return self.thisptr.getMeshType()
        def __set__(self, mesh_type):
            self.thisptr.setMeshType(mesh_type)

    # matrix_solver
    property matrix_solver:
        def __get__(self):
            return self.thisptr.getMatrixSolver()
        def __set__(self, matrix_solver):
            self.thisptr.setMatrixSolver(matrix_solver)

    # frequency
    property frequency:
        def __get__(self):
            return self.thisptr.getFrequency()
        def __set__(self, frequency):
            self.thisptr.setFrequency(frequency)

    # time_step_method
    property time_step_method:
        def __get__(self):
            return self.thisptr.getTimeStepMethod()
        def __set__(self, time_step_method):
            self.thisptr.setTimeStepMethod(time_step_method)

    # time_method_order
    property time_method_order:
        def __get__(self):
            return self.thisptr.getTimeMethodOrder()
        def __set__(self, time_method_order):
            self.thisptr.setTimeMethodOrder(time_method_order)

    # time_method_tolerance
    property time_method_tolerance:
        def __get__(self):
            return self.thisptr.getTimeMethodTolerance()
        def __set__(self, time_method_tolerance):
            self.thisptr.setTimeMethodTolerance(time_method_tolerance)

    # time_total
    property time_total:
        def __get__(self):
            return self.thisptr.getTimeTotal()
        def __set__(self, time_total):
            self.thisptr.setTimeTotal(time_total)

    # time_steps
    property time_steps:
        def __get__(self):
            return self.thisptr.getNumConstantTimeSteps()
        def __set__(self, steps):
            self.thisptr.setNumConstantTimeSteps(steps)

    # coupling type
    def get_coupling_type(self, source_field, target_field):
        return self.thisptr.getCouplingType(source_field, target_field)
    def set_coupling_type(self, source_field, target_field, type):
            self.thisptr.setCouplingType(source_field, target_field, type)

    # solve
    def solve(self):
        self.thisptr.solve()

__problem__ = __Problem__()
def problem(int clear = False):
    if (clear):
        __problem__.clear()
    return __problem__