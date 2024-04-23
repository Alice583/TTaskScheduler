#include <iostream>
#include <vector>
#include <cmath>
inline int count;

class TTaskScheduler {
private:
    class TaskWrapper {
    public:
        virtual void doFunction() = 0;

        virtual void* GetValue() = 0;
    };

    template<class Function, class Type>
    class Arguments0: public TaskWrapper {
    public:
        Arguments0(Function function) {
            function_ = function;
        }
        void doFunction() override {
            if (res_ready_) {
                return;
            }
            try {
                result_ = function_();
                res_ready_ = true;
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        void* GetValue() override {
            return static_cast<void*>(&result_);
        }
    private:
        Function function_;
        Type result_ = NULL;
        bool res_ready_ = false;
    };

    template<typename Function, typename Type>
    class Arguments1: public TaskWrapper {
    public:
        Arguments1(Function f, Type val_a, TaskWrapper* relation) {
            function_ = f;
            a_ = val_a;
            relation_id_ = relation;
        }

        void doFunction() override {
            if (res_ready_) {
                return;
            }
            try {
                if (relation_id_ == nullptr) {
                    result_ = function_(a_);
                    res_ready_ = true;
                }
                else {
                    relation_id_->doFunction();
                    a_ = *static_cast<Type*>(relation_id_->GetValue());
                    result_ = function_(a_);
                    relation_id_ = nullptr;
                    res_ready_ = true;
                }
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }

        void* GetValue() override {
            return static_cast<void*>(&result_);
        }
    private:
        Function function_;
        Type a_ = NULL;
        Type result_ = NULL;
        TaskWrapper* relation_id_ = nullptr;
        bool res_ready_ = false;
    };

    template<typename Function, typename Type>
    class Arguments2: public TaskWrapper {
    public:
        Arguments2(Function& f, Type& val_a, Type& val_b, TaskWrapper* relation) {
            function_ = f;
            a_ = val_a;
            b_ = val_b;
            relation_id_ = relation;
        }

        void doFunction() override {
            if (res_ready_) {
                return;
            }
            try {
                if (relation_id_ == nullptr) {
                    result_ = function_(a_, b_);
                    res_ready_ = true;
                }
                else {
                    relation_id_->doFunction();
                    b_ = *static_cast<Type*>(relation_id_->GetValue());
                    result_ = function_(a_, b_);
                    relation_id_ = nullptr;
                    res_ready_ = true;
                }
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }

        void* GetValue() override {
            return static_cast<void*>(&result_);
        }
    private:
        Function function_;
        Type a_ = NULL;
        Type b_ = NULL;
        Type result_ = NULL;
        TaskWrapper* relation_id_ = nullptr;
        bool res_ready_ = false;
    };

    std::vector<TaskWrapper*> tasks_;
    std::vector<TaskWrapper*> relations_;

public:
    template<typename Function>
    auto add(Function func) {
        using Type = decltype(func());
        if (count > 0 && relations_[count] != nullptr) {
            auto* task = new Arguments0<Function, Type>(func);
            tasks_.push_back(task);
            count++;
            return tasks_[tasks_.size() - 1];
        }
        auto* task = new Arguments0<Function, Type>(func);
        tasks_.push_back(task);
        count++;
        return tasks_[tasks_.size() - 1];
    }

    template<typename Function, typename Type>
    auto add(Function func, Type a) {
        if (count > 0 && relations_[count] != nullptr) {
            auto* task = new Arguments1<Function, Type>(func, a, relations_[count]);
            tasks_.push_back(task);
            count++;
            return tasks_[tasks_.size() - 1];
        }
        auto* task = new Arguments1<Function, Type>(func, a, nullptr);

        tasks_.push_back(task);
        count++;
        return tasks_[tasks_.size() - 1];
    }

    template<typename Function, typename Type>
    auto add(Function func, Type a, Type b) {
        if (count > 0 && relations_[count] != nullptr) {
            auto* task = new Arguments2<Function, Type>(func, a, b, relations_[count]);
            tasks_.push_back(task);
            count++;
            return tasks_[tasks_.size() - 1];
        }
        auto* task = new Arguments2<Function, Type>(func, a, b, nullptr);
        tasks_.push_back(task);
        count++;
        return tasks_[tasks_.size() - 1];
    }

    void executeAll() {
        try {
            for (int i = tasks_.size() - 1; i >= 0; i--) {
                tasks_[i]->doFunction();
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    template<typename Type>
    auto getResult(TaskWrapper* id) {
        void* res = id->GetValue();
        return *static_cast<Type*>(res);
    }

    template<typename Type>
    Type getFutureResult(TaskWrapper* id) {
        relations_.resize(count + 1);
        relations_[count] = id;
        return *static_cast<Type*>(id->GetValue());
    }

};