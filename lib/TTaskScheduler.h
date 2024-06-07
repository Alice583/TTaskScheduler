
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

inline int count;

class TTaskScheduler {
private:
    class TaskWrapper {
    public:
        virtual void doFunction() = 0;
        virtual void* getValue() = 0;
        virtual ~TaskWrapper() = default;
    };

    template<class Function, class Type>
    class Arguments0 : public TaskWrapper {
    public:
        Arguments0(Function function) : function_(function), res_ready_(false), result_() {}

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

        void* getValue() override {
            return static_cast<void*>(&result_);
        }

    private:
        Function function_;
        Type result_;
        bool res_ready_;
    };

    template<typename Function, typename Type>
    class Arguments1 : public TaskWrapper {
    public:
        Arguments1(Function f, Type val_a, std::shared_ptr<TaskWrapper> relation)
                : function_(f), a_(val_a), relation_id_(relation), res_ready_(false), result_() {}

        void doFunction() override {
            if (res_ready_) {
                return;
            }
            try {
                if (!relation_id_) {
                    result_ = function_(a_);
                    res_ready_ = true;
                } else {
                    relation_id_->doFunction();
                    a_ = *static_cast<Type*>(relation_id_->getValue());
                    result_ = function_(a_);
                    relation_id_ = nullptr;
                    res_ready_ = true;
                }
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }

        void* getValue() override {
            return static_cast<void*>(&result_);
        }

    private:
        Function function_;
        Type a_;
        Type result_;
        std::shared_ptr<TaskWrapper> relation_id_;
        bool res_ready_;
    };

    template<typename Function, typename Type>
    class Arguments2 : public TaskWrapper {
    public:
        Arguments2(Function f, Type val_a, Type val_b, std::shared_ptr<TaskWrapper> relation)
                : function_(f), a_(val_a), b_(val_b), relation_id_(relation), res_ready_(false), result_() {}

        void doFunction() override {
            if (res_ready_) {
                return;
            }
            try {
                if (!relation_id_) {
                    result_ = function_(a_, b_);
                    res_ready_ = true;
                } else {
                    relation_id_->doFunction();
                    b_ = *static_cast<Type*>(relation_id_->getValue());
                    result_ = function_(a_, b_);
                    relation_id_ = nullptr;
                    res_ready_ = true;
                }
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }

        void* getValue() override {
            return static_cast<void*>(&result_);
        }

    private:
        Function function_;
        Type a_;
        Type b_;
        Type result_;
        std::shared_ptr<TaskWrapper> relation_id_;
        bool res_ready_;
    };

    std::vector<std::shared_ptr<TaskWrapper>> tasks_;
    std::vector<std::shared_ptr<TaskWrapper>> relations_;

public:
    template<typename Function>
    auto add(Function func) {
        using Type = decltype(func());
        auto task = std::make_shared<Arguments0<Function, Type>>(func);
        if (count > 0 && relations_[count] != nullptr) {
            tasks_.push_back(task);
            count++;
            return tasks_.back();
        }
        tasks_.push_back(task);
        count++;
        return tasks_.back();
    }

    template<typename Function, typename Type>
    auto add(Function func, Type a) {
        auto task = std::make_shared<Arguments1<Function, Type>>(func, a, relations_.empty() ? nullptr : relations_[count]);
        tasks_.push_back(task);
        count++;
        return tasks_.back();
    }

    template<typename Function, typename Type>
    auto add(Function func, Type a, Type b) {
        auto task = std::make_shared<Arguments2<Function, Type>>(func, a, b, relations_.empty() ? nullptr : relations_[count]);
        tasks_.push_back(task);
        count++;
        return tasks_.back();
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
    auto getResult(std::shared_ptr<TaskWrapper> id) {
        void* res = id->getValue();
        return *static_cast<Type*>(res);
    }

    template<typename Type>
    Type getFutureResult(std::shared_ptr<TaskWrapper> id) {
        relations_.resize(count + 1);
        relations_[count] = id;
        return *static_cast<Type*>(id->getValue());
    }
};