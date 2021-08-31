auto filterDeliveryD = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto edt = %sdt + hours(24 * 365);
    return %sdt <= dt && dt < edt;
});

GroupBy([], 
        [sum($0:double)],
    Project([$0.amount:double],
        Filter($0.quantity == %quantity,
            Filter(filterDeliveryD($0.delivery_d:datetime),
                NodeScan("OrderLine")
            )
        )
    )
)