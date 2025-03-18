auto makeUniqueWrapper = [](auto&&... args) {
    return std::make_unique<std::decay_t<decltype(args)>>...(
        std::forward<decltype(args)>(args)...
    );
};
auto ptr = makeUniqueWrapper(42); // Creates unique_ptr<int>

